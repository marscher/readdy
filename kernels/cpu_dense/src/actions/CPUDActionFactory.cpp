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
 * @file ProgramFactory.cpp
 * @brief << brief description >>
 * @author clonker
 * @date 23.11.16
 */


#include <readdy/kernel/cpu_dense/actions/CPUDActionFactory.h>
#include <readdy/kernel/cpu_dense/actions/CPUDEulerBDIntegrator.h>
#include <readdy/kernel/cpu_dense/actions/CPUDUpdateNeighborList.h>
#include <readdy/kernel/cpu_dense/actions/CPUDCalculateForces.h>
#include <readdy/kernel/cpu_dense/actions/CPUDEvaluateCompartments.h>
#include <readdy/kernel/cpu_dense/actions/reactions/CPUDGillespie.h>
#include <readdy/kernel/cpu_dense/actions/reactions/CPUDUncontrolledApproximation.h>
#include <readdy/kernel/cpu_dense/actions/reactions/CPUDGillespieParallel.h>

namespace core_p = readdy::model::actions;

namespace readdy {
namespace kernel {
namespace cpu_dense {
namespace actions {
CPUDActionFactory::CPUDActionFactory(CPUDKernel *const kernel) : kernel(kernel) {}

core_p::EulerBDIntegrator *CPUDActionFactory::createEulerBDIntegrator(double timeStep) const {
    return new CPUDEulerBDIntegrator(kernel, timeStep);
}

core_p::CalculateForces *CPUDActionFactory::createCalculateForces() const {
    return new CPUDCalculateForces(kernel);
}

core_p::UpdateNeighborList *
CPUDActionFactory::createUpdateNeighborList(core_p::UpdateNeighborList::Operation operation,
                                             double skinSize) const {
    return new CPUDUpdateNeighborList(kernel, operation, skinSize);
}

core_p::EvaluateCompartments *CPUDActionFactory::createEvaluateCompartments() const {
    return new CPUDEvaluateCompartments(kernel);
}

core_p::reactions::UncontrolledApproximation *
CPUDActionFactory::createUncontrolledApproximation(double timeStep) const {
    return new reactions::CPUDUncontrolledApproximation(kernel, timeStep);
}

core_p::reactions::Gillespie *CPUDActionFactory::createGillespie(double timeStep) const {
    return new reactions::CPUDGillespie(kernel, timeStep);
}

core_p::reactions::GillespieParallel *CPUDActionFactory::createGillespieParallel(double timeStep) const {
    return new reactions::CPUDGillespieParallel(kernel, timeStep);
}

core_p::reactions::NextSubvolumes *CPUDActionFactory::createNextSubvolumes(double) const {
    log::critical("CPU_Dense kernel does not support the \"{}\" action",
                             core_p::getActionName<core_p::reactions::NextSubvolumes>());
    return nullptr;
}

readdy::model::actions::AddParticles *
CPUDActionFactory::createAddParticles(const std::vector<readdy::model::Particle> &particles) const {
    return new readdy::model::actions::AddParticles(kernel, particles);
}

readdy::model::actions::top::EvaluateTopologyReactions *
CPUDActionFactory::createEvaluateTopologyReactions(double timeStep) const {
    return nullptr;
}
}
}
}
}