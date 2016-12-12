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
 * @file SingleCPUUpdateNeighborList.h
 * @brief << brief description >>
 * @author clonker
 * @date 11.07.16
 */

#ifndef READDY_MAIN_SINGLECPUUPDATENEIGHBORLIST_H
#define READDY_MAIN_SINGLECPUUPDATENEIGHBORLIST_H

#include <readdy/model/programs/Programs.h>
#include <readdy/kernel/singlecpu/SCPUKernel.h>

namespace readdy {
namespace kernel {
namespace scpu {
namespace programs {

class SCPUUpdateNeighborList : public readdy::model::programs::UpdateNeighborList {

public:
    SCPUUpdateNeighborList(SCPUKernel *kernel);

    virtual void execute() override;

private:
    SCPUKernel *kernel;
};

}
}
}
}

#endif //READDY_MAIN_SINGLECPUUPDATENEIGHBORLIST_H