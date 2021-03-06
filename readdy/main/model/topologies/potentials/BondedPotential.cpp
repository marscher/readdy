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
 * @file BondPotential.cpp
 * @brief << brief description >>
 * @author clonker
 * @date 27.01.17
 * @copyright GNU Lesser General Public License v3.0
 */

#include <readdy/model/topologies/Topology.h>

namespace readdy {
namespace model {
namespace top {
namespace pot {

/*
 * Super class
 */

BondedPotential::BondedPotential(Topology *const topology, const bonds_t &bonds)
        : TopologyPotential(topology), bonds(bonds) {}

const BondedPotential::bonds_t &BondedPotential::getBonds() const {
    return bonds;
}

/*
 * Harmonic bond
 */

HarmonicBondPotential::HarmonicBondPotential(Topology *const topology, const bonds_t &bonds)
        : BondedPotential(topology, bonds) {
    const auto n = topology->getNParticles();
    for(const auto& bond : bonds) {
        if (bond.idx1 >= n) {
            throw std::invalid_argument("the first particle (" + std::to_string(bond.idx1) + ") was out of bounds!");
        }
        if (bond.idx2 >= n) {
            throw std::invalid_argument("the second particle (" + std::to_string(bond.idx2) + ") was out of bounds!");
        }
    }
}

double HarmonicBondPotential::calculateEnergy(const Vec3 &x_ij, const BondConfiguration &bond) const {
    const auto norm = std::sqrt(x_ij * x_ij);
    return bond.forceConstant * (norm - bond.length) * (norm - bond.length);
}

void
HarmonicBondPotential::calculateForce(Vec3 &force, const Vec3 &x_ij, const BondConfiguration &bond) const {
    const auto norm = std::sqrt(x_ij * x_ij);
    force += (2. * bond.forceConstant * (norm - bond.length) / norm) * x_ij;
}

std::unique_ptr<EvaluatePotentialAction>
HarmonicBondPotential::createForceAndEnergyAction(const TopologyActionFactory *const factory) {
    return factory->createCalculateHarmonicBondPotential(this);
}

BondConfiguration::BondConfiguration(std::size_t idx1, std::size_t idx2, double forceConstant, double length)
        : idx1(idx1), idx2(idx2), length(length), forceConstant(forceConstant) {}
}
}
}
}
