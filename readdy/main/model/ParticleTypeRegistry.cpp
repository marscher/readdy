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
 * @file ParticleTypeRegistry.cpp
 * @brief << brief description >>
 * @author clonker
 * @date 29.03.17
 * @copyright GNU Lesser General Public License v3.0
 */

#include <readdy/model/ParticleTypeRegistry.h>

namespace readdy {
namespace model {


ParticleTypeInfo::ParticleTypeInfo(const std::string &name, const double diffusionConstant, const double radius,
                                   const Particle::flavor_t flavor, const Particle::type_type typeId)
        : name(name), diffusionConstant(diffusionConstant), radius(radius), flavor(flavor), typeId(typeId) {}


const ParticleTypeInfo &ParticleTypeRegistry::info_of(const std::string &name) const {
    return info_of(type_mapping_.at(name));
}

const ParticleTypeInfo &ParticleTypeRegistry::info_of(const Particle::type_type type) const {
    return particle_info_.at(type);
}

const ParticleTypeRegistry::type_mapping_t &ParticleTypeRegistry::type_mapping() const {
    return type_mapping_;
}

std::string ParticleTypeRegistry::name_of(particle_type_type id) const {
    for (const auto &e : type_mapping_) {
        if (e.second == id) return e.first;
    }
    return "";
}

std::vector<particle_type_type> ParticleTypeRegistry::types_flat() const {
    std::vector<particle_type_type> v;
    for (auto &&entry : type_mapping_) {
        v.push_back(entry.second);
    }
    return v;
}

double ParticleTypeRegistry::radius_of(const particle_type_type type) const {
    return particle_info_.at(type).radius;
}

double ParticleTypeRegistry::radius_of(const std::string &type) const {
    return radius_of(id_of(type));
}

double ParticleTypeRegistry::diffusion_constant_of(const particle_type_type particleType) const {
    return particle_info_.at(particleType).diffusionConstant;
}

double ParticleTypeRegistry::diffusion_constant_of(const std::string &particleType) const {
    return diffusion_constant_of(id_of(particleType));
}

void
ParticleTypeRegistry::add(const std::string &name, const double diffusionConst, const double radius,
                          const Particle::flavor_t flavor) {
    particle_type_type t_id = type_counter_++;
    type_mapping_.emplace(name, t_id);
    particle_info_.emplace(std::make_pair(t_id, ParticleTypeInfo{name, diffusionConst, radius, flavor, t_id}));
    n_types_++;
}

particle_type_type ParticleTypeRegistry::id_of(const std::string &name) const {
    return type_mapping_.at(name);
}

const std::size_t &ParticleTypeRegistry::n_types() const {
    return n_types_;
}
}
}