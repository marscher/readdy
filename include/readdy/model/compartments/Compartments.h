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
 * @file Compartments.h
 * @brief << brief description >>
 * @author chrisfroe
 * @date 13.01.17
 * @copyright GNU Lesser General Public License v3.0
 */

#pragma once

#include "Compartment.h"

NAMESPACE_BEGIN(readdy)
NAMESPACE_BEGIN(model)
NAMESPACE_BEGIN(compartments)

class Sphere : public Compartment {
public:
    Sphere(const std::unordered_map<particleType_t, particleType_t> &conversions, const std::string &uniqueName, const Vec3 &origin,
           const double radius, const bool largerOrLess);

    virtual const bool isContained(const Vec3 &position) const override;

protected:
    const Vec3 origin;
    const double radius;
    const double radiusSquared;
    const bool largerOrLess;
};


class Plane : public Compartment {
public:
    Plane(const std::unordered_map<particleType_t, particleType_t> &conversions, const std::string &uniqueName, const Vec3 &normalCoefficients,
          const double distance, const bool largerOrLess);

    virtual const bool isContained(const Vec3 &position) const override;

protected:
    const Vec3 normalCoefficients;
    const double distanceFromOrigin;
    const bool largerOrLess;
};

template<typename T>
const std::string getCompartmentTypeName(typename std::enable_if<std::is_base_of<Sphere, T>::value>::type * = 0) {
    return "Sphere";
}

template<typename T>
const std::string getCompartmentTypeName(typename std::enable_if<std::is_base_of<Plane, T>::value>::type * = 0) {
    return "Plane";
}

NAMESPACE_END(compartments)
NAMESPACE_END(model)
NAMESPACE_END(readdy)
