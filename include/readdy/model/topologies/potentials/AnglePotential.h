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
 * @file AnglePotential.h
 * @brief << brief description >>
 * @author clonker
 * @date 26.01.17
 * @copyright GNU Lesser General Public License v3.0
 */

#pragma once

#include <cstddef>
#include <tuple>
#include <vector>
#include "TopologyPotential.h"

NAMESPACE_BEGIN(readdy)
NAMESPACE_BEGIN(model)
NAMESPACE_BEGIN(top)
NAMESPACE_BEGIN(pot)

class AnglePotential : public TopologyPotential {
public:
    using angles_t = std::vector<std::tuple<std::size_t, std::size_t, std::size_t>>;

    AnglePotential(Topology *const topology);

    virtual ~AnglePotential() = default;
};

struct AngleConfiguration {

    AngleConfiguration(size_t idx1, size_t idx2, size_t idx3, double forceConstant, double equilibriumAngle);

    const std::size_t idx1, idx2, idx3;
    const double equilibriumAngle, forceConstant;
};


class HarmonicAnglePotential : public AnglePotential {
public:
    using angle_t = AngleConfiguration;
    using angles_t = std::vector<AngleConfiguration>;

    HarmonicAnglePotential(Topology *const topology, const angles_t &angles);

    virtual ~HarmonicAnglePotential() = default;

    virtual std::unique_ptr<EvaluatePotentialAction>
    createForceAndEnergyAction(const TopologyActionFactory *const factory) override;

    const angles_t &getAngles() const;

    double calculateEnergy(const Vec3 &x_ji, const Vec3 &x_jk, const angle_t &angle) const;

    void
    calculateForce(Vec3 &f_i, Vec3 &f_j, Vec3 &f_k, const Vec3 &x_ji, const Vec3 &x_jk, const angle_t &angle) const;

protected:
    angles_t angles;
};

NAMESPACE_END(pot)
NAMESPACE_END(top)
NAMESPACE_END(model)
NAMESPACE_END(readdy)
