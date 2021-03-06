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
 * @file CPUTestParallelizedGillespie.cpp
 * @brief << brief description >>
 * @author clonker
 * @date 01.09.16
 */

#include <gtest/gtest.h>
#include <readdy/kernel/cpu/CPUKernel.h>
#include <readdy/kernel/cpu/nl/NeighborList.h>

namespace {

TEST(TestParallelGillespie, Sanity) {
    readdy::kernel::cpu::CPUKernel kernel;
    kernel.getKernelContext().setBoxSize(10, 10, 11);
    kernel.getKernelContext().particle_types().add("A", 10.0, 1.);
    kernel.registerReaction<readdy::model::reactions::Fusion>("Fusion", "A", "A", "A", 10, 1.0);
    kernel.addParticle("A", {-5, .2, -5.5});
    kernel.addParticle("A", {-5, .2, 5.5});
    kernel.addParticle("A", {-5, .2, 0});
    kernel.getKernelContext().configure();
    kernel.getCPUKernelStateModel().getNeighborList()->set_up();
    auto prog = kernel.createAction<readdy::model::actions::reactions::GillespieParallel>(1);
    prog->perform();
}
}