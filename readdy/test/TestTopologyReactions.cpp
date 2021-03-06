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
 * @file TestTopologyReactions.cpp
 * @brief << brief description >>
 * @author clonker
 * @date 29.03.17
 * @copyright GNU Lesser General Public License v3.0
 */

#include <gtest/gtest.h>
#include <readdy/model/topologies/GraphTopology.h>
#include <readdy/testing/KernelTest.h>
#include <readdy/testing/Utils.h>
#include <readdy/model/topologies/Utils.h>

namespace {

using particle_t = readdy::model::Particle;
using topology_particle_t = readdy::model::TopologyParticle;

class TestTopologyReactions : public KernelTest {
protected:
    virtual void SetUp() override {
        if (kernel->supportsTopologies()) {
            auto &ctx = kernel->getKernelContext();
            ctx.particle_types().add("Topology A", 1.0, 1.0, particle_t::FLAVOR_TOPOLOGY);
            ctx.particle_types().add("Topology B", 1.0, 1.0, particle_t::FLAVOR_TOPOLOGY);
            ctx.particle_types().add("Topology Invalid Type", 1.0, 1.0, particle_t::FLAVOR_TOPOLOGY);
            ctx.particle_types().add("A", 1.0, 1.0, particle_t::FLAVOR_NORMAL);

            ctx.configureTopologyBondPotential("Topology A", "Topology A", {10, 10});
            ctx.configureTopologyBondPotential("Topology A", "Topology B", {10, 10});
            ctx.configureTopologyBondPotential("Topology B", "Topology B", {10, 10});

            ctx.setBoxSize(10, 10, 10);
        }
    }

};

static readdy::model::top::GraphTopology* setUpSmallTopology(readdy::model::Kernel* kernel) {
    auto &ctx = kernel->getKernelContext();
    topology_particle_t x_0{0, 0, 0, ctx.particle_types().id_of("Topology A")};
    topology_particle_t x_1{0, 0, 0, ctx.particle_types().id_of("Topology A")};
    topology_particle_t x_2{0, 0, 0, ctx.particle_types().id_of("Topology A")};
    auto topology = kernel->getKernelStateModel().addTopology({x_0, x_1, x_2});
    {
        auto it = topology->graph().vertices().begin();
        auto it2 = ++topology->graph().vertices().begin();
        topology->graph().addEdge(it, it2);
        std::advance(it, 1);
        std::advance(it2, 1);
        topology->graph().addEdge(it, it2);
    }
    return topology;
}

TEST(TestTopologyReactions, ModeFlags) {
    using namespace readdy;
    using namespace readdy::model::top;

    reactions::TopologyReaction::reaction_function rfun = [](GraphTopology &t) -> reactions::Recipe {
        reactions::TopologyReaction::reaction_recipe recipe {t};
        return recipe;
    };
    reactions::TopologyReaction topologyReaction (rfun, [](const GraphTopology &) {
        return 0;
    });
    topologyReaction.expect_connected_after_reaction();
    ASSERT_TRUE(topologyReaction.expects_connected_after_reaction());
    ASSERT_FALSE(topologyReaction.creates_child_topologies_after_reaction());

    topologyReaction.create_child_topologies_after_reaction();
    EXPECT_FALSE(topologyReaction.expects_connected_after_reaction());
    EXPECT_TRUE(topologyReaction.creates_child_topologies_after_reaction());

    topologyReaction.raise_if_invalid();
    EXPECT_TRUE(topologyReaction.raises_if_invalid());
    EXPECT_FALSE(topologyReaction.rolls_back_if_invalid());

    topologyReaction.roll_back_if_invalid();
    EXPECT_FALSE(topologyReaction.raises_if_invalid());
    EXPECT_TRUE(topologyReaction.rolls_back_if_invalid());
}

TEST_P(TestTopologyReactions, ChangeParticleType) {
    using namespace readdy;
    if (!kernel->supportsTopologies()) {
        log::debug("kernel {} does not support topologies, thus skipping the test", kernel->getName());
        return;
    }
    auto topology = setUpSmallTopology(kernel.get());
    const auto &types = kernel->getKernelContext().particle_types();
    topology->graph().setVertexLabel(topology->graph().vertices().begin(), "begin");
    {
        auto reactionFunction = [&](model::top::GraphTopology &top) {
            model::top::reactions::Recipe recipe (top);
            recipe.changeParticleType(top.graph().vertices().begin(), types.id_of("Topology B"));
            return recipe;
        };
        model::top::reactions::TopologyReaction reaction {reactionFunction, 5};
        reaction.expect_connected_after_reaction();
        reaction.raise_if_invalid();
        topology->addReaction(reaction);
    }
    {
        auto reactionFunction = [&](model::top::GraphTopology &top) {
            model::top::reactions::Recipe recipe (top);
            recipe.changeParticleType("begin", types.id_of("Topology A"));
            return recipe;
        };
        auto rateFunction = [&](const model::top::GraphTopology &top) {
            return 15;
        };
        model::top::reactions::TopologyReaction reaction {reactionFunction, rateFunction};
        reaction.expect_connected_after_reaction();
        reaction.raise_if_invalid();
        topology->addReaction(reaction);
    }
    topology->updateReactionRates();
    auto& r1 = *topology->registeredReactions().begin();
    auto& r2 = *(topology->registeredReactions().begin() + 1);
    EXPECT_EQ(std::get<1>(r1), 5) << "Expected (constant) rate: 5";
    EXPECT_EQ(std::get<1>(r2), 15) << "Expected (function) rate: 15";

    {
        auto result = std::get<0>(r1).execute(*topology, kernel.get());
        ASSERT_EQ(result.size(), 0) << "reaction is in-place, expect empty return vector";
        auto particles = kernel->getKernelStateModel().getParticlesForTopology(*topology);
        auto v = topology->graph().namedVertexPtr("begin");
        ASSERT_EQ(particles[v->particleIndex].getType(), types.id_of("Topology B"));
        ASSERT_EQ(v->particleType(), particles[v->particleIndex].getType()) << "expect that the particle type in "
                            "the graph representation and the particle data coincide";
    }
    {
        auto result = std::get<0>(r2).execute(*topology, kernel.get());
        ASSERT_EQ(result.size(), 0) << "reaction is in-place, expect empty return vector";
        auto particles = kernel->getKernelStateModel().getParticlesForTopology(*topology);
        auto v = topology->graph().namedVertexPtr("begin");
        ASSERT_EQ(particles[v->particleIndex].getType(), types.id_of("Topology A"));
        ASSERT_EQ(v->particleType(), particles[v->particleIndex].getType()) << "expect that the particle type in "
                            "the graph representation and the particle data coincide";
    }
}

TEST_P(TestTopologyReactions, GEXF) {
    using namespace readdy;
    if(!kernel->supportsTopologies()) {
        log::debug("kernel {} does not support topologies, thus skipping the test", kernel->getName());
        return;
    }
    auto topology = setUpSmallTopology(kernel.get());
    auto middle = ++topology->graph().vertices().begin();
    topology->graph().setVertexLabel(middle, "middle");
    auto gexf = model::top::util::to_gexf(topology->graph());
    EXPECT_TRUE(gexf.find("<node id=\"1\" label=\"middle\"") != std::string::npos) << "middle node should be labelled";
    EXPECT_TRUE(gexf.find("source=\"0\" target=\"1\"") != std::string::npos) << "first two vertices are connected";
}

TEST_P(TestTopologyReactions, AddEdgeNamed) {
    // add edge between first and last vertex, creating a circular structure x_2 -> x_0 <-> x_1 <-> x_2 <- x_0
    using namespace readdy;
    if (!kernel->supportsTopologies()) {
        log::debug("kernel {} does not support topologies, thus skipping the test", kernel->getName());
        return;
    }
    auto topology = setUpSmallTopology(kernel.get());
    const auto &types = kernel->getKernelContext().particle_types();
    topology->graph().setVertexLabel(topology->graph().vertices().begin(), "begin");
    topology->graph().setVertexLabel(--topology->graph().vertices().end(), "end");
    {
        auto reactionFunction = [&](model::top::GraphTopology &top) {
            model::top::reactions::Recipe recipe (top);
            recipe.addEdge("begin", "end");
            return recipe;
        };
        model::top::reactions::TopologyReaction reaction {reactionFunction, 5};
        reaction.expect_connected_after_reaction();
        reaction.raise_if_invalid();
        topology->addReaction(reaction);
    }
    topology->updateReactionRates();
    auto result = std::get<0>(topology->registeredReactions().back()).execute(*topology, kernel.get());
    ASSERT_EQ(result.size(), 0) << "reaction is in-place, expect empty return vector";
    EXPECT_TRUE(topology->graph().containsEdge(std::make_tuple("begin", "end")));
}

TEST_P(TestTopologyReactions, AddEdgeIterator) {
    // add edge between first and last vertex, creating a circular structure x_2 -> x_0 <-> x_1 <-> x_2 <- x_0
    using namespace readdy;
    if (!kernel->supportsTopologies()) {
        log::debug("kernel {} does not support topologies, thus skipping the test", kernel->getName());
        return;
    }
    auto topology = setUpSmallTopology(kernel.get());
    topology->graph().setVertexLabel(topology->graph().vertices().begin(), "begin");
    topology->graph().setVertexLabel(--topology->graph().vertices().end(), "end");
    {
        auto reactionFunction = [&](model::top::GraphTopology &top) {
            model::top::reactions::Recipe recipe (top);
            recipe.addEdge(std::make_tuple(top.graph().vertices().begin(), --top.graph().vertices().end()));
            return recipe;
        };
        model::top::reactions::TopologyReaction reaction {reactionFunction, 5};
        reaction.expect_connected_after_reaction();
        reaction.raise_if_invalid();
        topology->addReaction(reaction);
    }
    topology->updateReactionRates();
    auto result = std::get<0>(topology->registeredReactions().back()).execute(*topology, kernel.get());
    ASSERT_EQ(result.size(), 0) << "reaction is in-place, expect empty return vector";
    EXPECT_TRUE(topology->graph().containsEdge(std::make_tuple("begin", "end")));
}


TEST_P(TestTopologyReactions, RemoveEdgeStraightforwardCase) {
    using namespace readdy;
    if (!kernel->supportsTopologies()) {
        log::debug("kernel {} does not support topologies, thus skipping the test", kernel->getName());
        return;
    }
    auto topology = setUpSmallTopology(kernel.get());

    {
        auto reactionFunction = [&](model::top::GraphTopology &top) {
            model::top::reactions::Recipe recipe (top);
            recipe.removeEdge(top.graph().vertices().begin(), ++top.graph().vertices().begin());
            return recipe;
        };
        model::top::reactions::TopologyReaction reaction {reactionFunction, 5};
        reaction.create_child_topologies_after_reaction();
        reaction.raise_if_invalid();
        topology->addReaction(reaction);
    }
    topology->updateReactionRates();
    model::top::Topology::particles_t particles;
    {
        std::copy(topology->getParticles().begin(), topology->getParticles().end(), std::back_inserter(particles));
    }
    auto result = std::get<0>(topology->registeredReactions().back()).execute(*topology, kernel.get());
    ASSERT_EQ(result.size(), 2);

    const auto& top1 = result.at(0);
    const auto& top2 = result.at(1);

    ASSERT_EQ(top1.getNParticles(), 1) << "first topology should have only 1 particle";
    ASSERT_EQ(top2.getNParticles(), 2) << "second topology should have 2 particles";
    ASSERT_EQ(top2.graph().vertices().begin()->neighbors().size(), 1) << "second topology has one edge";
    ASSERT_EQ((++top2.graph().vertices().begin())->neighbors().size(), 1) << "second topology has one edge";
    ASSERT_EQ(top2.graph().vertices().begin()->neighbors().at(0), ++top2.graph().vertices().begin())
                                << "second topology has one edge";
    ASSERT_EQ((++top2.graph().vertices().begin())->neighbors().at(0), top2.graph().vertices().begin())
                                << "second topology has one edge";
    ASSERT_EQ(top1.graph().vertices().begin()->particleIndex, 0)
                                << "particle indices are topology relative and should begin with 0";
    ASSERT_EQ(top2.graph().vertices().begin()->particleIndex, 0)
                                << "particle indices are topology relative and should begin with 0";
    ASSERT_EQ((++top2.graph().vertices().begin())->particleIndex, 1)
                                << "particle indices are topology relative and should begin with 0";

    // check if particle mappings are still valid
    ASSERT_EQ(top1.getParticles().at(0), particles.at(0));
    ASSERT_EQ(top2.getParticles().at(0), particles.at(1));
    ASSERT_EQ(top2.getParticles().at(1), particles.at(2));
}

TEST_P(TestTopologyReactions, RemoveEdgeRollback) {
    using namespace readdy;
    if (!kernel->supportsTopologies()) {
        log::debug("kernel {} does not support topologies, thus skipping the test", kernel->getName());
        return;
    }
    auto topology = setUpSmallTopology(kernel.get());

    {
        auto reactionFunction = [&](model::top::GraphTopology &top) {
            model::top::reactions::Recipe recipe (top);
            recipe.removeEdge(top.graph().vertices().begin(), ++top.graph().vertices().begin());
            return recipe;
        };
        model::top::reactions::TopologyReaction reaction {reactionFunction, 5};
        reaction.expect_connected_after_reaction();
        reaction.roll_back_if_invalid();
        topology->addReaction(reaction);
    }
    topology->updateReactionRates();
    model::top::Topology::particles_t particles;
    {
        std::copy(topology->getParticles().begin(), topology->getParticles().end(), std::back_inserter(particles));
    }
    std::vector<model::top::GraphTopology> result;
    {
        log::Level level (spdlog::level::err);
        result = std::get<0>(topology->registeredReactions().back()).execute(*topology, kernel.get());
    }
    const auto& graph = topology->graph();
    const auto& vertices = graph.vertices();
    EXPECT_TRUE(result.empty());
    EXPECT_TRUE(graph.containsEdge(vertices.begin(), ++vertices.begin()));
    EXPECT_TRUE(graph.containsEdge(++vertices.begin(), --vertices.end()));
}

TEST_P(TestTopologyReactions, SplitUpChain) {
    using namespace readdy;
    if (!kernel->supportsTopologies()) {
        log::debug("kernel {} does not support topologies, thus skipping the test", kernel->getName());
        return;
    }

    std::size_t n_chain_elements = 50;
    auto &ctx = kernel->getKernelContext();

    ctx.setBoxSize(10, 10, 10);
    std::vector<readdy::model::TopologyParticle> topologyParticles;
    {
        topologyParticles.reserve(n_chain_elements);
        for (std::size_t i = 0; i < n_chain_elements; ++i) {
            const auto id = ctx.particle_types().id_of("Topology A");
            topologyParticles.emplace_back(-5 + i * 10. / static_cast<readdy::scalar>(n_chain_elements), 0, 0, id);
        }
    }
    auto topology = kernel->getKernelStateModel().addTopology(topologyParticles);
    {
        auto it = topology->graph().vertices().begin();
        auto it2 = ++topology->graph().vertices().begin();
        while(it2 != topology->graph().vertices().end()) {
            topology->graph().addEdge(it, it2);
            std::advance(it, 1);
            std::advance(it2, 1);
        }
    }

    {
        auto reactionFunction = [&](model::top::GraphTopology &top) {
            model::top::reactions::Recipe recipe (top);
            auto& vertices = top.graph().vertices();
            int current_n_vertices = (int) vertices.size();
            if(current_n_vertices > 1) {
                auto edge = readdy::model::rnd::uniform_int<>(0, current_n_vertices-2);
                auto it1 = vertices.begin();
                auto it2 = ++vertices.begin();
                for(int i = 0; i < edge; ++i) {
                    ++it1;
                    ++it2;
                }
                recipe.removeEdge(it1, it2);
            }
            return recipe;
        };
        auto rateFunction = [](const model::top::GraphTopology &top) {
            return top.getNParticles() > 1 ? top.getNParticles()/50. : 0;
        };
        model::top::reactions::TopologyReaction reaction {reactionFunction, rateFunction};
        reaction.create_child_topologies_after_reaction();
        reaction.roll_back_if_invalid();
        topology->addReaction(reaction);
    }

    {
        auto integrator = kernel->getActionFactory().createIntegrator("EulerBDIntegrator", 1.0);
        auto forces = kernel->getActionFactory().createAction<readdy::model::actions::CalculateForces>();
        auto topReactions = kernel->getActionFactory().createAction<readdy::model::actions::top::EvaluateTopologyReactions>(1.0);

        std::size_t time = 0;
        std::size_t n_time_steps = 500;

        kernel->initialize();

        forces->perform();
        kernel->evaluateObservables(time);
        for(time = 1; time < n_time_steps; ++time) {
            integrator->perform();
            topReactions->perform();
            forces->perform();
            kernel->evaluateObservables(time);
        }
        kernel->finalize();
    }

    auto topologies = kernel->getKernelStateModel().getTopologies();
    for(const auto topPtr : topologies) {
        EXPECT_EQ(topPtr->getNParticles(), 1);
        EXPECT_EQ(topPtr->graph().vertices().size(), 1);
    }

}

TEST_P(TestTopologyReactions, SplitUpChainDecay) {
    using namespace readdy;
    if (!kernel->supportsTopologies()) {
        log::debug("kernel {} does not support topologies, thus skipping the test", kernel->getName());
        return;
    }

    std::size_t n_chain_elements = 50;
    auto &ctx = kernel->getKernelContext();

    ctx.setBoxSize(10, 10, 10);
    std::vector<readdy::model::TopologyParticle> topologyParticles;
    {
        topologyParticles.reserve(n_chain_elements);
        for (std::size_t i = 0; i < n_chain_elements; ++i) {
            const auto id = ctx.particle_types().id_of("Topology A");
            topologyParticles.emplace_back(-5 + i * 10. / static_cast<readdy::scalar>(n_chain_elements), 0, 0, id);
        }
    }
    auto topology = kernel->getKernelStateModel().addTopology(topologyParticles);
    {
        auto it = topology->graph().vertices().begin();
        auto it2 = ++topology->graph().vertices().begin();
        while(it2 != topology->graph().vertices().end()) {
            topology->graph().addEdge(it, it2);
            std::advance(it, 1);
            std::advance(it2, 1);
        }
    }

    {
        // split reaction
        auto reactionFunction = [&](model::top::GraphTopology &top) {
            model::top::reactions::Recipe recipe (top);
            auto& vertices = top.graph().vertices();
            int current_n_vertices = (int) vertices.size();
            if(current_n_vertices > 1) {
                auto edge = readdy::model::rnd::uniform_int<>(0, current_n_vertices-2);
                auto it1 = vertices.begin();
                auto it2 = ++vertices.begin();
                for(int i = 0; i < edge; ++i) {
                    ++it1;
                    ++it2;
                }
                recipe.removeEdge(it1, it2);
            }

            return recipe;
        };
        auto rateFunction = [](const model::top::GraphTopology &top) {
            return top.getNParticles() > 1 ? top.getNParticles()/50. : 0;
        };
        model::top::reactions::TopologyReaction reaction {reactionFunction, rateFunction};
        reaction.create_child_topologies_after_reaction();
        reaction.roll_back_if_invalid();

        topology->addReaction(reaction);
    }
    {
        // decay reaction
        auto reactionFunction = [&](model::top::GraphTopology &top) {
            model::top::reactions::Recipe recipe (top);
            if(top.graph().vertices().size() == 1) {
                recipe.changeParticleType(top.graph().vertices().begin(),
                                          kernel->getKernelContext().particle_types().id_of("A"));
            } else {
                throw std::logic_error("this reaction should only be executed when there is exactly "
                                               "one particle in the topology");
            }
            return recipe;
        };
        auto rateFunction = [](const model::top::GraphTopology &top) {
            return top.getNParticles() > 1 ? 0 : 1;
        };
        model::top::reactions::TopologyReaction reaction {reactionFunction, rateFunction};
        reaction.create_child_topologies_after_reaction();
        reaction.roll_back_if_invalid();
        topology->addReaction(reaction);
    }

    {
        auto integrator = kernel->getActionFactory().createIntegrator("EulerBDIntegrator", 1.0);
        auto forces = kernel->getActionFactory().createAction<readdy::model::actions::CalculateForces>();
        auto topReactions = kernel->getActionFactory().createAction<readdy::model::actions::top::EvaluateTopologyReactions>(1.0);

        std::size_t time = 0;
        std::size_t n_time_steps = 500;

        kernel->initialize();

        forces->perform();
        kernel->evaluateObservables(time);
        for(time = 1; time < n_time_steps; ++time) {
            integrator->perform();
            topReactions->perform();
            forces->perform();
            kernel->evaluateObservables(time);

        }
        kernel->finalize();
    }

    EXPECT_EQ(kernel->getKernelStateModel().getTopologies().size(), 0);
}

INSTANTIATE_TEST_CASE_P(TestTopologyReactionsKernelTests, TestTopologyReactions,
                        ::testing::ValuesIn(readdy::testing::getKernelsToTest()));

}
