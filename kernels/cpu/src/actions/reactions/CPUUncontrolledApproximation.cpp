/********************************************************************
 * Copyright © 2016 Computational Molecular Biology Group,          *
 *                  Freie Universität Berlin (GER)                  *
 *                                                                  *
 * This file is part of ReaDDy.                                     *
 *                                                                  *
 * ReaDDy is free software: you can redistribute it and/or modify   *
 * it under the terms of the GNU Lesser General Public License as   *
 * published by the Free Software Foundation, either version 3 of   *
 * the License, or (at your option) any later version.              *
 *                                                                  *
 * This program is distributed in the hope that it will be useful,  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    *
 * GNU Lesser General Public License for more details.              *
 *                                                                  *
 * You should have received a copy of the GNU Lesser General        *
 * Public License along with this program. If not, see              *
 * <http://www.gnu.org/licenses/>.                                  *
 ********************************************************************/


/**
 * << detailed description >>
 *
 * @file UncontrolledApproximation.cpp
 * @brief << brief description >>
 * @author clonker
 * @date 20.10.16
 */

#include <future>

#include <readdy/kernel/cpu/actions/reactions/CPUUncontrolledApproximation.h>
#include <readdy/kernel/cpu/actions/reactions/Event.h>
#include <readdy/kernel/cpu/actions/reactions/ReactionUtils.h>

namespace readdy {
namespace kernel {
namespace cpu {
namespace actions {
namespace reactions {

namespace thd = readdy::util::thread;

using event_t = Event;
using data_t = neighbor_list::data_t;
using data_iter_t = data_t::const_iterator;
using neighbor_list_iter_t = neighbor_list::const_iterator;
using entry_type = data_t::Entry;

using event_future_t = std::future<std::vector<event_t>>;
using event_promise_t = std::promise<std::vector<event_t>>;

CPUUncontrolledApproximation::CPUUncontrolledApproximation(CPUKernel *const kernel, double timeStep)
        : super(timeStep), kernel(kernel) {

}

void findEvents(std::size_t, data_iter_t begin, data_iter_t end, neighbor_list_iter_t nl_begin,
                const CPUKernel *const kernel, double dt, bool approximateRate, event_promise_t &events,
                std::promise<std::size_t> &n_events) {
    std::vector<event_t> eventsUpdate;
    const auto &data = *kernel->getCPUKernelStateModel().getParticleData();
    const auto &d2 = kernel->getKernelContext().getDistSquaredFun();
    auto it = begin;
    auto it_nl = nl_begin;
    auto index = static_cast<std::size_t>(std::distance(data.begin(), begin));
    for (; it != end; ++it, ++it_nl, ++index) {
        const auto &entry = *it;
        // this being false should really not happen, though
        if (!entry.is_deactivated()) {
            // order 1
            {
                const auto &reactions = kernel->getKernelContext().reactions().order1_by_type(entry.type);
                for (auto it_reactions = reactions.begin(); it_reactions != reactions.end(); ++it_reactions) {
                    const auto rate = (*it_reactions)->getRate();
                    if (rate > 0 && shouldPerformEvent(rate, dt, approximateRate)) {
                        eventsUpdate.push_back(
                                {1, (*it_reactions)->getNProducts(), index, index, rate, 0,
                                 static_cast<event_t::reaction_index_type>(it_reactions - reactions.begin()),
                                 entry.type, 0});
                    }
                }
            }
            // order 2
            for (const auto idx_neighbor : *it_nl) {
                if (index > idx_neighbor) continue;
                const auto &neighbor = data.entry_at(idx_neighbor);
                const auto &reactions = kernel->getKernelContext().reactions().order2_by_type(entry.type,
                                                                                              neighbor.type);
                if (!reactions.empty()) {
                    const auto distSquared = d2(neighbor.position(), entry.position());
                    for (auto it_reactions = reactions.begin(); it_reactions < reactions.end(); ++it_reactions) {
                        const auto &react = *it_reactions;
                        const auto rate = react->getRate();
                        if (rate > 0 && distSquared < react->getEductDistanceSquared()
                            && shouldPerformEvent(rate, dt, approximateRate)) {
                            const auto reaction_index = static_cast<event_t::reaction_index_type>(it_reactions -
                                                                                                  reactions.begin());
                            eventsUpdate.push_back(
                                    {2, react->getNProducts(), index, idx_neighbor, rate, 0, reaction_index,
                                     entry.type, neighbor.type});
                        }
                    }
                }
            }
        }
    }

    n_events.set_value(eventsUpdate.size());
    events.set_value(std::move(eventsUpdate));
}

void CPUUncontrolledApproximation::perform() {
    const auto &ctx = kernel->getKernelContext();
    const auto &fixPos = ctx.getFixPositionFun();
    auto &stateModel = kernel->getCPUKernelStateModel();
    auto &data = *stateModel.getParticleData();
    auto &nl = *kernel->getCPUKernelStateModel().getNeighborList();

    if (ctx.recordReactionsWithPositions()) {
        kernel->getCPUKernelStateModel().reactionRecords().clear();
    }
    if (ctx.recordReactionCounts()) {
        readdy::model::observables::ReactionCounts::initializeCounts(stateModel.reactionCounts(), ctx);
    }

    // gather events
    std::vector<std::future<std::size_t>> n_eventsFutures;
    std::vector<std::promise<std::size_t>> n_events_promises(kernel->getNThreads());
    std::vector<event_future_t> eventFutures;
    std::vector<event_promise_t> promises(kernel->getNThreads());
    {

        const auto &executor = kernel->executor();

        std::vector<std::function<void(std::size_t)>> executables;
        executables.reserve(kernel->getNThreads());

        const std::size_t grainSize = data.size() / kernel->getNThreads();

        auto it = data.cbegin();
        auto it_nl = nl.cbegin();
        for (unsigned int i = 0; i < kernel->getNThreads() - 1; ++i) {
            eventFutures.push_back(promises.at(i).get_future());
            n_eventsFutures.push_back(n_events_promises.at(i).get_future());

            executables.push_back(executor.pack(findEvents, it, it + grainSize, it_nl, kernel, timeStep, true,
                                                std::ref(promises.at(i)), std::ref(n_events_promises.at(i))));
            it += grainSize;
            it_nl += grainSize;
        }
        {
            auto &eventPromise = promises.back();
            eventFutures.push_back(eventPromise.get_future());
            auto &n_events = n_events_promises.back();
            n_eventsFutures.push_back(n_events.get_future());


            executables.push_back(executor.pack(findEvents, it, data.cend(), it_nl, kernel, timeStep, true,
                                                std::ref(eventPromise), std::ref(n_events)));
        }
        executor.execute_and_wait(std::move(executables));
    }

    // collect events
    std::vector<event_t> events;
    {
        std::size_t n_events = 0;
        for (auto &&f : n_eventsFutures) {
            n_events += f.get();
        }
        events.reserve(n_events);
        for (auto &&f : eventFutures) {
            auto eventUpdate = std::move(f.get());
            auto mBegin = std::make_move_iterator(eventUpdate.begin());
            auto mEnd = std::make_move_iterator(eventUpdate.end());
            events.insert(events.end(), mBegin, mEnd);
        }
    }

    // shuffle reactions
    std::random_shuffle(events.begin(), events.end());

    // execute reactions
    {
        data_t::entries_update_t newParticles{};
        std::vector<data_t::index_t> decayedEntries{};

        // todo better conflict detection?
        for (auto it = events.begin(); it != events.end(); ++it) {
            auto &event = *it;
            if (event.cumulativeRate == 0) {
                auto entry1 = event.idx1;
                if (event.nEducts == 1) {
                    auto reaction = ctx.reactions().order1_by_type(event.t1)[event.reactionIdx];
                    if (ctx.recordReactionsWithPositions()) {
                        record_t record;
                        record.reactionIndex = event.reactionIdx;
                        performReaction(data, ctx, entry1, entry1, newParticles, decayedEntries, reaction, &record);
                        fixPos(record.where);
                        kernel->getCPUKernelStateModel().reactionRecords().push_back(std::move(record));
                    } else {
                        performReaction(data, ctx, entry1, entry1, newParticles, decayedEntries, reaction, nullptr);
                    }
                    if (ctx.recordReactionCounts()) {
                        auto &countsOrder1 = std::get<0>(stateModel.reactionCounts());
                        countsOrder1.at(event.t1).at(event.reactionIdx)++;
                    }
                    for (auto _it2 = it + 1; _it2 != events.end(); ++_it2) {
                        if (_it2->idx1 == entry1 || _it2->idx2 == entry1) {
                            _it2->cumulativeRate = 1;
                        }
                    }
                } else {
                    auto reaction = ctx.reactions().order2_by_type(event.t1, event.t2)[event.reactionIdx];
                    if (ctx.recordReactionsWithPositions()) {
                        record_t record;
                        record.reactionIndex = event.reactionIdx;
                        performReaction(data, ctx, entry1, event.idx2, newParticles, decayedEntries, reaction, &record);
                        fixPos(record.where);
                        kernel->getCPUKernelStateModel().reactionRecords().push_back(std::move(record));
                    } else {
                        performReaction(data, ctx, entry1, event.idx2, newParticles, decayedEntries, reaction, nullptr);
                    }
                    if (ctx.recordReactionCounts()) {
                        auto &countsOrder2 = std::get<1>(stateModel.reactionCounts());
                        countsOrder2.at(std::tie(event.t1, event.t2)).at(event.reactionIdx)++;
                    }
                    for (auto _it2 = it + 1; _it2 != events.end(); ++_it2) {
                        if (_it2->idx1 == entry1 || _it2->idx2 == entry1 ||
                            _it2->idx1 == event.idx2 || _it2->idx2 == event.idx2) {
                            _it2->cumulativeRate = 1;
                        }
                    }
                }
            }
        }

        nl.updateData(std::make_pair(std::move(newParticles), std::move(decayedEntries)));
    }
}
}
}
}
}
}