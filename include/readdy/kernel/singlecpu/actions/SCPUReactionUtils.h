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
 * @file SCPUReactionUtils.h
 * @brief << brief description >>
 * @author clonker
 * @date 31.01.17
 * @copyright GNU Lesser General Public License v3.0
 */

#ifndef READDY_MAIN_SCPUREACTIONUTILS_H
#define READDY_MAIN_SCPUREACTIONUTILS_H
namespace readdy {
namespace kernel {
namespace scpu {
namespace actions {
namespace reactions {

using data_t = readdy::kernel::scpu::model::SCPUParticleData;
using reaction_type = readdy::model::reactions::Reaction<1>::ReactionType;
using ctx_t = readdy::model::KernelContext;
using fix_pos = ctx_t::fix_pos_fun;

template<typename Reaction>
void performReaction(data_t &data, data_t::index_t idx1, data_t::index_t idx2, data_t::entries_update_t &newEntries,
                     std::vector<data_t::index_t> &decayedEntries, Reaction *reaction, const fix_pos &fixPos) {
    switch (reaction->getType()) {
        case reaction_type::Decay: {
            decayedEntries.push_back(idx1);
            break;
        }
        case reaction_type::Conversion: {
            data.entry_at(idx1).type = reaction->getProducts()[0];
            break;
        }
        case reaction_type::Enzymatic: {
            auto &entry1 = data.entry_at(idx1);
            if (entry1.type == reaction->getEducts()[1]) {
                // p1 is the catalyst
                data.entry_at(idx2).type = reaction->getProducts()[0];
            } else {
                // p2 is the catalyst
                entry1.type = reaction->getProducts()[0];
            }
            break;
        }
        case reaction_type::Fission: {
            auto &entry1 = data.entry_at(idx1);
            auto n3 = readdy::model::rnd::normal3(0, 1);
            n3 /= std::sqrt(n3 * n3);

            readdy::model::Particle p(entry1.position() - reaction->getWeight2() * reaction->getProductDistance() * n3,
                                      reaction->getProducts()[1]);
            newEntries.push_back({p});

            entry1.type = reaction->getProducts()[0];
            entry1.pos += reaction->getWeight1() * reaction->getProductDistance() * n3;
            fixPos(entry1.pos);
            break;
        }
        case reaction_type::Fusion: {
            auto &entry1 = data.entry_at(idx1);
            const auto e1Pos = data.entry_at(idx1).pos;
            const auto e2Pos = data.entry_at(idx2).pos;
            if (reaction->getEducts()[0] == entry1.type) {
                entry1.pos += reaction->getWeight1() * (e2Pos - e1Pos);
            } else {
                entry1.pos += reaction->getWeight1() * (e1Pos - e2Pos);
            }
            fixPos(entry1.pos);
            entry1.type = reaction->getProducts()[0];
            decayedEntries.push_back(idx2);
            break;
        }
    }
}
}
}
}
}
}
#endif //READDY_MAIN_SCPUREACTIONUTILS_H