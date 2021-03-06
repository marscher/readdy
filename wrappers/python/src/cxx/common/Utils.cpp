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
 * @file Utils.cpp
 * @brief << brief description >>
 * @author clonker
 * @date 20.07.17
 * @copyright GNU Lesser General Public License v3.0
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <spdlog/fmt/ostr.h>
#include <readdy/io/File.h>

#include <readdy/model/observables/io/TrajectoryEntry.h>
#include <readdy/model/observables/io/Types.h>
#include <readdy/model/IOUtils.h>
#include <fstream>

namespace py = pybind11;
using rvp = py::return_value_policy;

using radiusmap = std::map<std::string, readdy::scalar>;

void
convert_xyz(const std::string &h5name, const std::string &trajName, const std::string &out, bool generateTcl = true, bool tclRuler = false,
            const radiusmap &radii = {}) {
    readdy::log::debug(R"(converting "{}" to "{}")", h5name, out);
    readdy::io::blosc_compression::initialize();

    readdy::io::File f(h5name, readdy::io::File::Action::OPEN, readdy::io::File::Flag::READ_ONLY);
    auto &rootGroup = f.getRootGroup();

    // get particle types from config
    std::vector<readdy::model::ioutils::ParticleTypeInfo> types;
    {
        auto config = rootGroup.subgroup("readdy/config");
        config.read("particle_types", types, readdy::model::ioutils::ParticleTypeInfoMemoryType(),
                    readdy::model::ioutils::ParticleTypeInfoFileType());
    }

    // map from type name to max number of particles in traj
    std::unordered_map<readdy::particle_type_type, std::size_t> maxCounts;
    for (const auto &type : types) {
        maxCounts[type.type_id] = 0;
        readdy::log::debug("got type {} with id {} and D {}", type.name, type.type_id, type.diffusion_constant);
    }

    auto traj = rootGroup.subgroup("readdy/trajectory/" + trajName);

    // limits
    std::vector<std::size_t> limits;
    traj.read("limits", limits, readdy::io::STDDataSetType<std::size_t>(),
              readdy::io::NativeDataSetType<std::size_t>());
    // records
    std::vector<readdy::model::observables::TrajectoryEntry> entries;
    readdy::model::observables::util::TrajectoryEntryMemoryType memoryType;
    readdy::model::observables::util::TrajectoryEntryFileType fileType;
    traj.read("records", entries, memoryType, fileType);

    std::unordered_map<readdy::particle_type_type, std::size_t> typeMapping(types.size());
    {
        std::size_t i = 0;
        for (const auto &type : types) {
            typeMapping[type.type_id] = i++;
        }
    }

    {
        std::vector<std::size_t> currentCounts(types.size());
        for (std::size_t i = 0; i < limits.size(); i += 2) {
            std::fill(currentCounts.begin(), currentCounts.end(), 0);

            auto begin = limits[i];
            auto end = limits[i + 1];

            for (auto it = entries.begin() + begin; it != entries.begin() + end; ++it) {
                currentCounts[typeMapping.at(it->typeId)]++;
            }

            for (const auto &e : typeMapping) {
                maxCounts[e.first] = std::max(currentCounts.at(e.second), maxCounts[e.first]);
            }
        }
    }

    readdy::log::debug("got particle counts:");
    for (const auto &e : maxCounts) {
        readdy::log::debug("\t type id {}: max {} particles", e.first, e.second);
    }

    std::size_t maxParticlesSum = 0;
    {
        for (const auto &e :maxCounts) {
            maxParticlesSum += e.second;
        }
    }

    readdy::log::debug("writing to xyz (n timesteps {})", limits.size() / 2);

    {
        std::fstream fs;
        fs.exceptions(std::fstream::failbit);
        fs.open(out, std::fstream::out | std::fstream::trunc);

        std::vector<std::size_t> currentCounts(types.size());
        std::vector<std::string> xyzPerType(types.size());
        for (std::size_t frame = 0; frame < limits.size(); frame += 2) {

            // number of atoms + comment line (empty)
            fs << maxParticlesSum << std::endl << std::endl;

            std::fill(currentCounts.begin(), currentCounts.end(), 0);
            std::fill(xyzPerType.begin(), xyzPerType.end(), "");

            auto begin = limits[frame];
            auto end = limits[frame + 1];

            for (auto it = entries.begin() + begin; it != entries.begin() + end; ++it) {
                currentCounts[typeMapping.at(it->typeId)]++;
                auto &currentXYZ = xyzPerType.at(typeMapping.at(it->typeId));
                currentXYZ += "type_" + std::to_string(it->typeId) + "\t" + std::to_string(it->pos.x) + "\t" +
                              std::to_string(it->pos.y) + "\t" + std::to_string(it->pos.z) + "\n";
            }

            for (const auto &mappingEntry : typeMapping) {
                auto nGhosts = maxCounts.at(mappingEntry.first) - currentCounts.at(mappingEntry.second);

                fs << xyzPerType.at(mappingEntry.second);
                for (int x = 0; x < nGhosts; ++x) {
                    fs << "type_" + std::to_string(mappingEntry.first) + "\t0\t0\t0\n";
                }
            }
        }

    }

    if (generateTcl) {
        readdy::log::debug("generating tcl script file {}.tcl", out);

        std::fstream fs;
        fs.exceptions(std::fstream::failbit);
        fs.open(out + ".tcl", std::fstream::out | std::fstream::trunc);

        fs << "mol delete top" << std::endl;
        fs << "mol load xyz " << out << std::endl;
        fs << "mol delrep 0 top" << std::endl;
        fs << "display resetview" << std::endl;

        if (tclRuler) {
            fs << "display projection orthographic" << std::endl;
            fs << "ruler grid" << std::endl;
        }

        std::size_t i = 0;
        for (const auto &t : types) {
            auto radius = static_cast<readdy::scalar>(1.0);
            {
                auto it = radii.find(t.name);
                if (it == radii.end()) {
                    readdy::log::warn("type {} had explicitly no specified radius, using 1.0", t.name);
                } else {
                    radius = it ->second;
                }
            }
            fs << "mol representation VDW " << std::to_string(radius * .7) << " 16.0" << std::endl;
            fs << "mol selection name type_" + std::to_string(t.type_id) << std::endl;
            fs << "mol color ColorID " << (i++) << std::endl;
            fs << "mol addrep top" << std::endl;
        }
        fs << "animate goto 0" << std::endl;
        fs << "color Display Background white" << std::endl;
        fs << "molinfo top set {center_matrix} {{{1 0 0 0}{0 1 0 0}{0 0 1 0}{0 0 0 1}}}" << std::endl;
    }

    readdy::log::debug("converting finished");
}

void exportUtils(py::module &m) {
    using namespace pybind11::literals;
    m.def("convert_xyz", &convert_xyz, "h5_file_name"_a, "traj_data_set_name"_a, "xyz_out_file_name"_a,
          "generate_tcl"_a = true, "tcl_with_grid"_a = false, "radii"_a = radiusmap{});
}