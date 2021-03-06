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
 * @file TopologyReaction.cpp
 * @brief << brief description >>
 * @author clonker
 * @date 03.04.17
 * @copyright GNU Lesser General Public License v3.0
 */

#include <readdy/model/topologies/reactions/TopologyReaction.h>

#include <readdy/model/Kernel.h>
#include <readdy/model/topologies/Utils.h>
#include <readdy/model/topologies/reactions/TopologyReactionException.h>

namespace readdy {
namespace model {
namespace top {
namespace reactions {

TopologyReaction::TopologyReaction(const reaction_function& reaction_function, const rate_function &rate_function)
        : _reaction_function(reaction_function)
        , _rate_function(rate_function) { }


double TopologyReaction::rate(const GraphTopology &topology) const {
    return _rate_function(topology);
}

TopologyReaction::reaction_recipe TopologyReaction::operations(GraphTopology &topology) const {
    return _reaction_function(topology);
}

const bool TopologyReaction::raises_if_invalid() const {
    return mode_.flags.test(mode::raise_or_rollback_flag);
}

void TopologyReaction::raise_if_invalid() {
    mode_.raise();
}

const bool TopologyReaction::rolls_back_if_invalid() const {
    return !raises_if_invalid();
}

void TopologyReaction::roll_back_if_invalid() {
    mode_.rollback();
}

const bool TopologyReaction::expects_connected_after_reaction() const {
    return mode_.flags.test(mode::expect_connected_or_create_children_flag);
}

void TopologyReaction::expect_connected_after_reaction() {
    mode_.expect_connected();
}

const bool TopologyReaction::creates_child_topologies_after_reaction() const {
    return !expects_connected_after_reaction();
}

void TopologyReaction::create_child_topologies_after_reaction() {
    mode_.create_children();
}

TopologyReaction::TopologyReaction(const TopologyReaction::reaction_function &reaction_function, const double &rate)
        : TopologyReaction(reaction_function, [rate](const GraphTopology&) -> scalar { return rate; }) {}

std::vector<GraphTopology> TopologyReaction::execute(GraphTopology &topology, const Kernel* const kernel) {
    auto recipe = operations(topology);
    auto& steps = recipe.steps();
    if(!steps.empty()) {
        auto topologyActionFactory = kernel->getTopologyActionFactory();
        std::vector<op::Operation::action_ptr> actions;
        actions.reserve(steps.size());
        for (auto &op : steps) {
            actions.push_back(op->create_action(&topology, topologyActionFactory));
        }

        bool exceptionOccurred = false;
        {
            // perform reaction
            auto it = actions.begin();
            try {
                for (; it != actions.end(); ++it) {
                    (*it)->execute();
                }
            } catch (const TopologyReactionException &exception) {
                log::warn("exception occurred while executing topology reaction: {}", exception.what());
                exceptionOccurred = true;
                if (raises_if_invalid()) {
                    throw;
                }
            }
            if (exceptionOccurred && rolls_back_if_invalid() && it != actions.begin()) {
                log::warn("rolling back...");
                for (;; --it) {
                    (*it)->undo();
                    if (it == actions.begin()) break;
                }
            }
        }
        if (!exceptionOccurred) {
            // post reaction
            if (expects_connected_after_reaction()) {
                bool valid = true;
                if (!topology.graph().isConnected()) {
                    // we expected it to be connected after the reaction.. but it is not, raise or rollback.
                    log::warn("The topology was expected to still be connected after the reaction, but it was not.");
                    valid = false;
                }
                {
                    // check if all particle types are topology flavored
                    const auto &types = kernel->getKernelContext().particle_types();
                    for (const auto &v : topology.graph().vertices()) {
                        if (types.info_of(v.particleType()).flavor != Particle::FLAVOR_TOPOLOGY) {
                            log::warn("The topology contained particles that were not topology flavored.");
                            valid = false;
                        }
                    }
                }
                if (!valid) {
                    log::warn("GEXF representation: {}", util::to_gexf(topology.graph()));
                    if (rolls_back_if_invalid()) {
                        log::warn("rolling back...");
                        for (auto it = actions.rbegin(); it != actions.rend(); ++it) {
                            (*it)->undo();
                        }
                    } else {
                        throw TopologyReactionException(
                                "The topology was invalid after the reaction, see previous warning messages.");
                    }
                } else {
                    // if valid, update force field
                    topology.configure();
                    // and update reaction rates
                    topology.updateReactionRates();
                }
            } else {
                if (!topology.graph().isConnected()) {
                    auto subTopologies = topology.connectedComponents();
                    assert(subTopologies.size() > 1 && "This should be at least 2 as the graph is not connected.");
                    return std::move(subTopologies);
                } else {
                    // if valid, update force field
                    topology.configure();
                    // and update reaction rates
                    topology.updateReactionRates();
                }
            }
        }
    }
    return {};
}


void Mode::raise() {
    flags[raise_or_rollback_flag] = true;
}

void Mode::rollback() {
    flags[raise_or_rollback_flag] = false;
}

void Mode::expect_connected() {
    flags[expect_connected_or_create_children_flag] = true;
}

void Mode::create_children() {
    flags[expect_connected_or_create_children_flag] = false;
}

}
}
}
}
