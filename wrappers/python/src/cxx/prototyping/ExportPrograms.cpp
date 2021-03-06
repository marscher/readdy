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
 * @file Programs.cpp.cpp
 * @brief << brief description >>
 * @author clonker
 * @date 03.08.16
 */


#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <readdy/kernel/singlecpu/actions/SCPUActionFactory.h>
#include <readdy/kernel/singlecpu/actions/SCPUEulerBDIntegrator.h>
#include <readdy/kernel/singlecpu/actions/SCPUCalculateForces.h>
#include <readdy/kernel/singlecpu/actions/SCPUUpdateNeighborList.h>
#include <readdy/kernel/singlecpu/actions/SCPUReactionImpls.h>
#include "ProgramWrap.h"

namespace py = pybind11;
using rvp = py::return_value_policy;
namespace rpy = readdy::rpy;

using particle_t = readdy::model::Particle;

using prog_factory_t = readdy::model::actions::ActionFactory;

using action_t = readdy::model::actions::Action;
using program_wrap_t = readdy::rpy::PyProgram;

using add_particle_t = readdy::model::actions::AddParticles;
using euler_integrator_t = readdy::kernel::scpu::actions::SCPUEulerBDIntegrator;
using forces_t = readdy::kernel::scpu::actions::SCPUCalculateForces;
using neighbor_list_t = readdy::kernel::scpu::actions::SCPUUpdateNeighborList;

using reactions_u_a_t = readdy::kernel::scpu::actions::reactions::SCPUUncontrolledApproximation;

void exportPrograms(py::module &proto) {
    using scpu_kernel_t = readdy::kernel::scpu::SCPUKernel;
    py::class_<prog_factory_t>(proto, "ActionFactory")
            .def("create_add_particles", [](const prog_factory_t& self, const std::vector<readdy::model::Particle> &p) {
                std::unique_ptr<action_t> action = self.createAction<add_particle_t>(p);
                return action.release();
            }, rvp::take_ownership)
            .def("create_euler_integrator", [](const prog_factory_t& self, double dt) {
                std::unique_ptr<action_t> action = self.createAction<readdy::model::actions::EulerBDIntegrator>(dt);
                return action.release();
            }, rvp::take_ownership)
            .def("create_update_forces", [](const prog_factory_t& self) {
                std::unique_ptr<action_t> action = self.createAction<readdy::model::actions::CalculateForces>();
                return action.release();
            }, rvp::take_ownership)
            .def("create_update_neighbor_list", [](const prog_factory_t& self) {
                std::unique_ptr<action_t> action = self.createAction<readdy::model::actions::UpdateNeighborList>();
                return action.release();
            }, rvp::take_ownership)
            .def("create_reactions_uncontrolled_approximation", [](const prog_factory_t & self, double dt) {
                std::unique_ptr<action_t> action = self.createAction<readdy::model::actions::reactions::UncontrolledApproximation>(dt);
                return action.release();
            }, rvp::take_ownership);

    py::class_<readdy::model::top::TopologyActionFactory> (proto, "TopologyActionFactory");

    py::class_ <action_t, program_wrap_t> program(proto, "Action");
    program
            .def(py::init<>())
            .def("perform", &action_t::perform);

    py::class_<add_particle_t>(proto, "AddParticles", program)
            .def("perform", &add_particle_t::perform);

    py::class_<readdy::model::actions::EulerBDIntegrator>(proto, "EulerBDIntegratorBase", program);
    py::class_<euler_integrator_t, readdy::model::actions::EulerBDIntegrator>(proto, "EulerBDIntegrator")
            .def(py::init<scpu_kernel_t *, double>())
            .def("perform", &euler_integrator_t::perform);

    py::class_<readdy::model::actions::CalculateForces>(proto, "CalculateForcesBase", program)
            .def("perform", &readdy::model::actions::CalculateForces::perform);
    py::class_<forces_t, readdy::model::actions::CalculateForces>(proto, "CalculateForces")
            .def(py::init<scpu_kernel_t *>())
            .def("perform", &forces_t::perform);

    py::class_<readdy::model::actions::UpdateNeighborList>(proto, "UpdateNeighborListBase", program);
    py::class_<neighbor_list_t, readdy::model::actions::UpdateNeighborList>(proto, "UpdateNeighborList")
            .def(py::init<scpu_kernel_t *>())
            .def("perform", &neighbor_list_t::perform);

    /**
     *
                 [](reactions_u_a_t &self, const std::string name, bpy::object handler) {
                     self.registerReactionScheme_11(name, py_fun_11_t(handler));
                 })
     */

    py::class_<readdy::model::actions::reactions::UncontrolledApproximation>(proto, "UncontrolledApproxBase", program);
    py::class_<reactions_u_a_t, readdy::model::actions::reactions::UncontrolledApproximation>(proto, "ReactionsUncontrolledApproximation")
            .def(py::init<scpu_kernel_t *, double>())
            .def("perform", &reactions_u_a_t::perform)
            .def("register_reaction_scheme_11", &reactions_u_a_t::registerReactionScheme_11)
            .def("register_reaction_scheme_12", &reactions_u_a_t::registerReactionScheme_12)
            .def("register_reaction_scheme_21", &reactions_u_a_t::registerReactionScheme_21)
            .def("register_reaction_scheme_22", &reactions_u_a_t::registerReactionScheme_22);
}