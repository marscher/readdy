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
 * @file SingleCPUUpdateNeighborList.cpp
 * @brief << brief description >>
 * @author clonker
 * @date 11.07.16
 */
#include <readdy/kernel/singlecpu/actions/SCPUUpdateNeighborList.h>

namespace core_actions = readdy::model::actions;

namespace readdy {
namespace kernel {
namespace scpu {
namespace actions {

void SCPUUpdateNeighborList::perform() {
    switch (operation) {
        case create:
            kernel->getKernelStateModel().updateNeighborList();
            break;
        case clear:
            kernel->getKernelStateModel().clearNeighborList();
            break;
    }
}

SCPUUpdateNeighborList::SCPUUpdateNeighborList(SCPUKernel *const kernel, core_actions::UpdateNeighborList::Operation op,
                                               double skinSize)
        : UpdateNeighborList(op, skinSize), kernel(kernel){
    if(skinSize >= 0) {
        log::warn("Ignoring skin size for single cpu kernel, as there is no Verlet list implementation");
    }
}

bool SCPUUpdateNeighborList::supportsSkin() const {
    return false;
}

}
}
}
}
