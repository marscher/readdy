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
 * Declaration of the base class for all order 2 potentials. They basically have calculateForce, calculateEnergy and
 * calculateForceAndEnergy methods, which take a modifiable reference argument and the difference vector x_ij between
 * two particles.
 * Further, subclasses have to implement getCutoffRadius so that the neighbor list can be created more efficiently.
 *
 * @file PotentialOrder2.h
 * @brief Declaration of the base class for all order 2 potentials.
 * @author clonker
 * @date 31.05.16
 */

#pragma once
#include "Potential.h"

NAMESPACE_BEGIN(readdy)
NAMESPACE_BEGIN(model)
class ParticleTypeRegistry;

NAMESPACE_BEGIN(potentials)
class PotentialRegistry;
class PotentialOrder2 : public Potential {
protected:
    using particle_type_type = readdy::model::Particle::type_type;
public:
    PotentialOrder2(const std::string& particleType1, const std::string& particleType2)
            : Potential(2), particleType1(particleType1), particleType2(particleType2) {}

    virtual double calculateEnergy(const Vec3 &x_ij) const = 0;

    virtual void calculateForce(Vec3 &force, const Vec3 &x_ij) const = 0;

    virtual void calculateForceAndEnergy(Vec3 &force, double &energy, const Vec3 &x_ij) const = 0;

    virtual double getCutoffRadius() const = 0;

    virtual double getCutoffRadiusSquared() const = 0;

    friend std::ostream &operator<<(std::ostream &os, const PotentialOrder2 &potential) {
        os << potential.describe();
        return os;
    }

protected:
    friend class readdy::model::potentials::PotentialRegistry;

    virtual void configureForTypes(const ParticleTypeRegistry* const, particle_type_type type1, particle_type_type type2) = 0;

    const std::string particleType1, particleType2;
};

NAMESPACE_END(potentials)
NAMESPACE_END(model)
NAMESPACE_END(readdy)
