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
 * @file Gillespie.h
 * @brief << brief description >>
 * @author clonker
 * @date 20.10.16
 */

#ifndef READDY_CPUKERNEL_GILLESPIE_H
#define READDY_CPUKERNEL_GILLESPIE_H

#include <readdy/kernel/cpu/CPUKernel.h>
#include <readdy/common/range.h>
#include "ReactionUtils.h"

namespace readdy {
namespace kernel {
namespace cpu {
namespace programs {
namespace reactions {

class CPUGillespie : public readdy::model::programs::reactions::Gillespie {
    using event_t = Event;
    using reaction_idx_t = event_t::index_type;

public:

    CPUGillespie(CPUKernel const *const kernel);

    virtual void execute() override;

protected:
    CPUKernel const *const kernel;
};
}
}
}
}
}

#endif //READDY_CPUKERNEL_GILLESPIE_H