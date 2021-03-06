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
 * @file ParticleIndexPair.h
 * @brief << brief description >>
 * @author clonker
 * @date 14.07.16
 */

#pragma once
#include <cstddef>
#include <stdexcept>
#include <ostream>
#include <readdy/common/hash.h>

namespace readdy {
namespace kernel {
namespace cpu {
namespace model {
struct ParticleIndexPair {
    std::size_t idx1, idx2;

    ParticleIndexPair(std::size_t idx1, std::size_t idx2) {
        if (idx1 < idx2) {
            ParticleIndexPair::idx1 = idx1;
            ParticleIndexPair::idx2 = idx2;
        } else if (idx1 > idx2) {
            ParticleIndexPair::idx1 = idx2;
            ParticleIndexPair::idx2 = idx1;
        } else {
            throw std::runtime_error("pair must not have equal indices");
        }
    }

    friend std::size_t hash_value(const ParticleIndexPair &pip) {
        std::size_t seed = 0;
        readdy::util::hash::combine(seed, pip.idx1);
        readdy::util::hash::combine(seed, pip.idx2);
        return seed;
    }

    friend bool operator==(const ParticleIndexPair &pip1, const ParticleIndexPair &pip2) {
        return pip1.idx1 == pip2.idx1 && pip1.idx2 == pip2.idx2;
    }

    friend std::ostream &operator<<(std::ostream &os, const ParticleIndexPair &pip) {
        os << "ParticleIndexPair(" << pip.idx1 << ", " << pip.idx2 << ")";
        return os;
    }
};

struct ParticleIndexPairHasher {
    std::size_t operator()(const ParticleIndexPair &pip) const {
        return hash_value(pip);
    }
};
}
}
}
}
