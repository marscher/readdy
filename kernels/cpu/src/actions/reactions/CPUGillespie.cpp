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
 * @file Gillespie.cpp
 * @brief << brief description >>
 * @author clonker
 * @date 20.10.16
 */
#include <readdy/kernel/cpu/actions/reactions/CPUGillespie.h>


namespace readdy {
namespace kernel {
namespace cpu {
namespace actions {
namespace reactions {

CPUGillespie::CPUGillespie(CPUKernel *const kernel, double timeStep) : super(timeStep), kernel(kernel) {}

void CPUGillespie::perform() {
    const auto &ctx = kernel->getKernelContext();
    auto &stateModel = kernel->getCPUKernelStateModel();
    auto data = stateModel.getParticleData();
    const auto &dist = ctx.getDistSquaredFun();
    const auto &fixPos = ctx.getFixPositionFun();
    const auto nl = stateModel.getNeighborList();

    if(ctx.recordReactionCounts()) {
        auto &countsOrder1 = stateModel.reactionCountsOrder1();
        auto &countsOrder2 = stateModel.reactionCountsOrder2();
        readdy::model::observables::util::initializeReactionCountMapping(countsOrder1, countsOrder2, ctx);
    }

    double alpha = 0.0;
    std::vector<event_t> events;
    gatherEvents(kernel, readdy::util::range<event_t::index_type>(0, data->size()), nl, *data, alpha, events, dist);
    if(ctx.recordReactionsWithPositions()) {
        stateModel.reactionRecords().clear();
        if(ctx.recordReactionCounts()) {
            auto particlesUpdate = handleEventsGillespie(kernel, timeStep, false, true, std::move(events), &stateModel.reactionRecords(),
                                                         &stateModel.reactionCountsOrder1(), &stateModel.reactionCountsOrder2());
            nl->updateData(std::move(particlesUpdate));
        } else {
            auto particlesUpdate = handleEventsGillespie(kernel, timeStep, false, true, std::move(events), &stateModel.reactionRecords(),
                                                         nullptr, nullptr);
            nl->updateData(std::move(particlesUpdate));
        }
    } else {
        if(ctx.recordReactionCounts()) {
            auto particlesUpdate = handleEventsGillespie(kernel, timeStep, false, true, std::move(events), nullptr,
                                                         &stateModel.reactionCountsOrder1(), &stateModel.reactionCountsOrder2());
            nl->updateData(std::move(particlesUpdate));
        } else {
            auto particlesUpdate = handleEventsGillespie(kernel, timeStep, false, true, std::move(events), nullptr,
                                                         nullptr, nullptr);
            nl->updateData(std::move(particlesUpdate));
        }
    }
}

}
}
}
}
}