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
 * @file ParticlesObservable.cpp
 * @brief << brief description >>
 * @author clonker
 * @date 13.03.17
 * @copyright GNU Lesser General Public License v3.0
 */

#include <readdy/model/observables/Particles.h>
#include <readdy/io/DataSet.h>
#include <readdy/model/observables/io/Types.h>
#include <readdy/model/observables/io/TimeSeriesWriter.h>

namespace readdy {
namespace model {
namespace observables {


struct Particles::Impl {
    std::unique_ptr<io::VLENDataSet> dataSetTypes;
    std::unique_ptr<io::VLENDataSet> dataSetIds;
    std::unique_ptr<io::VLENDataSet> dataSetPositions;
    std::unique_ptr<util::TimeSeriesWriter> time;
};

Particles::Particles(Kernel *const kernel, unsigned int stride) : Observable(kernel, stride),
                                                                  pimpl(std::make_unique<Impl>()) {}

void Particles::initializeDataSet(io::File &file, const std::string &dataSetName, unsigned int flushStride) {
    using particle_t = readdy::model::Particle;
    if (!pimpl->dataSetTypes) {

        std::vector<readdy::io::h5::dims_t> fs = {flushStride};
        std::vector<readdy::io::h5::dims_t> dims = {readdy::io::h5::UNLIMITED_DIMS};
        auto group = file.createGroup(std::string(util::OBSERVABLES_GROUP_PATH) + "/" + dataSetName);
        {
            pimpl->dataSetTypes = std::make_unique<io::VLENDataSet>(
                    group.createVLENDataSet<particle_type_type>("types", fs, dims));
        }
        {
            pimpl->dataSetIds = std::make_unique<io::VLENDataSet>(
                    group.createVLENDataSet<particle_t::id_type>("ids", fs, dims));
        }
        {
            pimpl->dataSetPositions = std::make_unique<io::VLENDataSet>(
                    group.createVLENDataSet("positions", fs, dims, io::NativeArrayDataSetType<Vec3::value_t, 3>(), io::STDArrayDataSetType<Vec3::value_t, 3>()));
        }
        pimpl->time = std::make_unique<util::TimeSeriesWriter>(group, flushStride);
    }
}

void Particles::append() {
    {
        auto &types = std::get<0>(result);
        log::debug("appending {} types ", types.size());
        for(auto t : types) {
            log::debug("    -> {}", t);
        }
        pimpl->dataSetTypes->append({1}, &types);
    }
    {
        auto &ids = std::get<1>(result);
        pimpl->dataSetIds->append({1}, &ids);
    }
    {
        pimpl->dataSetPositions->append({1}, &std::get<2>(result));
    }
    pimpl->time->append(t_current);
}

void Particles::flush() {
    if (pimpl->dataSetTypes) pimpl->dataSetTypes->flush();
    if (pimpl->dataSetIds) pimpl->dataSetIds->flush();
    if (pimpl->dataSetPositions) pimpl->dataSetPositions->flush();
    if (pimpl->time) pimpl->time->flush();
}

Particles::~Particles() = default;


}
}
}
