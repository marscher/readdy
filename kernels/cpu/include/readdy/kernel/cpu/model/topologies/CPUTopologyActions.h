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
 * @file CPUTopologyActions.h
 * @brief << brief description >>
 * @author clonker
 * @date 09.02.17
 * @copyright GNU Lesser General Public License v3.0
 */

#pragma once

#include <readdy/common/macros.h>
#include <readdy/model/topologies/Topology.h>
#include <readdy/model/topologies/potentials/TopologyPotentialActions.h>
#include <readdy/kernel/cpu/model/CPUParticleData.h>

NAMESPACE_BEGIN(readdy)
NAMESPACE_BEGIN(kernel)
NAMESPACE_BEGIN(cpu)
NAMESPACE_BEGIN(model)
NAMESPACE_BEGIN(top)

class CPUCalculateHarmonicBondPotential : public readdy::model::top::pot::CalculateHarmonicBondPotential {

    const harmonic_bond *const potential;
    CPUParticleData *const data;

public:
    CPUCalculateHarmonicBondPotential(const readdy::model::KernelContext *const context,
                                      CPUParticleData *const data,
                                      const harmonic_bond *const potential)
            : CalculateHarmonicBondPotential(context), potential(potential), data(data) {}

    virtual double perform() override {
        readdy::model::Vec3::value_t energy = 0;
        const auto &particleIndices = potential->getTopology()->getParticles();
        const auto &d = context->getShortestDifferenceFun();
        for (const auto &bond : potential->getBonds()) {
            readdy::model::Vec3 forceUpdate{0, 0, 0};
            auto &e1 = data->entry_at(particleIndices.at(bond.idx1));
            auto &e2 = data->entry_at(particleIndices.at(bond.idx2));
            const auto x_ij = d(e1.position(), e2.position());
            potential->calculateForce(forceUpdate, x_ij, bond);
            e1.force += forceUpdate;
            e2.force += -1 * forceUpdate;
            energy += potential->calculateEnergy(x_ij, bond);
        }
        return energy;
    }

};


class CPUCalculateHarmonicAnglePotential : public readdy::model::top::pot::CalculateHarmonicAnglePotential {
    const harmonic_angle *const potential;
    CPUParticleData *const data;
public:
    CPUCalculateHarmonicAnglePotential(const readdy::model::KernelContext *const context, CPUParticleData *const data,
                                       const harmonic_angle *const potential)
            : CalculateHarmonicAnglePotential(context), potential(potential), data(data) {}

    virtual double perform() override {
        readdy::model::Vec3::value_t energy = 0;
        const auto &particleIndices = potential->getTopology()->getParticles();
        const auto &d = context->getShortestDifferenceFun();


        for (const auto &angle : potential->getAngles()) {
            auto &e1 = data->entry_at(particleIndices.at(angle.idx1));
            auto &e2 = data->entry_at(particleIndices.at(angle.idx2));
            auto &e3 = data->entry_at(particleIndices.at(angle.idx3));
            const auto x_ji = d(e2.position(), e1.position());
            const auto x_jk = d(e2.position(), e3.position());
            energy += potential->calculateEnergy(x_ji, x_jk, angle);
            potential->calculateForce(e1.force, e2.force, e3.force, x_ji, x_jk, angle);
        }
        return energy;
    }
};

class CPUCalculateCosineDihedralPotential : public readdy::model::top::pot::CalculateCosineDihedralPotential {
    const cos_dihedral *const potential;
    CPUParticleData *const data;
public:
    CPUCalculateCosineDihedralPotential(const readdy::model::KernelContext *const context,
                                        CPUParticleData *const data,
                                        const cos_dihedral *const pot)
            : CalculateCosineDihedralPotential(context), potential(pot), data(data) {
    }

    virtual double perform() override {
        readdy::model::Vec3::value_t energy = 0;
        const auto &particleIndices = potential->getTopology()->getParticles();
        const auto &d = context->getShortestDifferenceFun();

        for (const auto &dih : potential->getDihedrals()) {
            auto &e_i = data->entry_at(particleIndices.at(dih.idx1));
            auto &e_j = data->entry_at(particleIndices.at(dih.idx2));
            auto &e_k = data->entry_at(particleIndices.at(dih.idx3));
            auto &e_l = data->entry_at(particleIndices.at(dih.idx4));
            const auto x_ji = d(e_j.position(), e_i.position());
            const auto x_kj = d(e_k.position(), e_j.position());
            const auto x_kl = d(e_k.position(), e_l.position());
            energy += potential->calculateEnergy(x_ji, x_kj, x_kl, dih);
            potential->calculateForce(e_i.force, e_j.force, e_k.force, e_l.force, x_ji, x_kj, x_kl, dih);
        }
        return energy;
    }
};

NAMESPACE_BEGIN(reactions)
NAMESPACE_BEGIN(op)

class CPUChangeParticleType : public readdy::model::top::reactions::actions::ChangeParticleType {
    CPUParticleData *const data;
public:
    CPUChangeParticleType(CPUParticleData *const data, top::GraphTopology *const topology, const vertex &v,
                          const particle_type_type &type_to) : ChangeParticleType(topology, v, type_to), data(data) {}

    virtual void execute() override {
        const auto idx = topology->getParticles().at(_vertex->particleIndex);
        _vertex->setParticleType(previous_type);
        std::swap(data->entry_at(idx).type, previous_type);
    }

    virtual void undo() override {
        execute();
    }

};

NAMESPACE_END(op)
NAMESPACE_END(reactions)

NAMESPACE_END(top)
NAMESPACE_END(model)
NAMESPACE_END(cpu)
NAMESPACE_END(kernel)
NAMESPACE_END(readdy)
