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
 * @file TopologyReactionRecipyBuilder.cpp
 * @brief << brief description >>
 * @author clonker
 * @date 13.04.17
 * @copyright GNU Lesser General Public License v3.0
 */

#include <readdy/model/topologies/reactions/Recipe.h>
#include <readdy/model/Kernel.h>

namespace readdy {
namespace model {
namespace top {
namespace reactions {

Recipe &Recipe::changeParticleType(const Recipe::label_vertex &of, const particle_type_type &to) {
    return changeParticleType(vertex_ref(&_topology.get().graph(), of), to);
}

Recipe::Recipe(GraphTopology &topology) : _topology(topology) {}

Recipe &Recipe::changeParticleType(const Recipe::vertex_ref &ref, const particle_type_type &to) {
    _steps.push_back(std::make_shared<op::ChangeParticleType>(ref, to));
    return *this;
}

Recipe &Recipe::addEdge(const Recipe::edge &edge) {
    _steps.push_back(std::make_shared<op::AddEdge>(edge));
    return *this;
}

Recipe &Recipe::addEdge(const label_edge &labels) {
    return addEdge(std::make_tuple(
            vertex_ref(&_topology.get().graph(), std::get<0>(labels)), vertex_ref(&_topology.get().graph(), std::get<1>(labels))
    ));
}

Recipe &Recipe::removeEdge(const Recipe::edge &edge) {
    _steps.push_back(std::make_shared<op::RemoveEdge>(edge));
    return *this;
}

Recipe &Recipe::removeEdge(const Recipe::label_edge &labels) {
    return removeEdge(std::make_tuple(
            vertex_ref(&_topology.get().graph(), std::get<0>(labels)), vertex_ref(&_topology.get().graph(), std::get<1>(labels))
    ));
}

Recipe &Recipe::separateVertex(const vertex_ref &vertex) {
    for(const auto& neighbor : vertex->neighbors()) {
        removeEdge(std::make_tuple(vertex, neighbor));
    }
    return *this;
}

const Recipe::reaction_operations &Recipe::steps() const {
    return _steps;
}

Recipe &Recipe::addEdge(const std::string &edge_label1, const std::string &edge_label2) {
    return addEdge(std::tie(edge_label1, edge_label2));
}

Recipe &Recipe::addEdge(Recipe::vertex_ref v1, Recipe::vertex_ref v2) {
    return addEdge(std::tie(v1, v2));
}

Recipe &Recipe::removeEdge(Recipe::vertex_ref v1, Recipe::vertex_ref v2) {
    return removeEdge(std::tie(v1, v2));
}

Recipe &Recipe::removeEdge(const std::string &label1, const std::string &label2) {
    return removeEdge(std::tie(label1, label2));
}

Recipe::topology_t &Recipe::topology() {
    return _topology;
}

const Recipe::topology_t &Recipe::topology() const {
    return _topology;
}

}
}
}
}