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


//
// Created by mho on 10/08/16.
//

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl_bind.h>

#include <readdy/model/Vec3.h>

#include <readdy/io/io.h>

namespace py = pybind11;

/**
 * Notice: Exporting classes here that are to be shared between prototyping and api module require the base
 * class to use be exported (preferably by the READDY_READDY_API macro defined in common/macros.h).
 */

void exportIO(py::module &);

void exportUtils(py::module& m);

void exportCommon(py::module& common) {
    common.def("set_logging_level", [](const std::string &level) -> void {
        readdy::log::console()->set_level([&level] {
            if (level == "trace") {
                return spdlog::level::trace;
            } else if (level == "debug") {
                return spdlog::level::debug;
            } else if (level == "info") {
                return spdlog::level::info;
            } else if (level == "warn") {
                return spdlog::level::warn;
            } else if (level == "err" || level == "error") {
                return spdlog::level::err;
            } else if (level == "critical") {
                return spdlog::level::critical;
            } else if (level == "off") {
                return spdlog::level::off;
            }
            readdy::log::warn("Did not select a valid logging level, setting to debug!");
            return spdlog::level::debug;
        }());
    }, "Function that sets the logging level. Possible arguments: \"trace\", \"debug\", \"info\", \"warn\", "
                       "\"err\", \"error\", \"critical\", \"off\".");
    common.def("register_blosc_hdf5_plugin", []() -> void {
        readdy::io::blosc_compression::initialize();
    });
    {
        py::module io = common.def_submodule("io", "ReaDDy IO module");
        exportIO(io);
    }
    {
        py::module util = common.def_submodule("util", "ReaDDy util module");
        exportUtils(util);
    }

    py::class_<readdy::model::Vec3>(common, "Vec")
            .def(py::init<double, double, double>())
            .def(py::self + py::self)
            .def(py::self - py::self)
            .def(double() * py::self)
            .def(py::self / double())
            .def(py::self += py::self)
            .def(py::self *= double())
            .def(py::self == py::self)
            .def(py::self != py::self)
            .def(py::self * py::self)
            .def("__repr__", [](const readdy::model::Vec3 &self) {
                std::ostringstream stream;
                stream << self;
                return stream.str();
            })
            .def("__getitem__", [](const readdy::model::Vec3 &self, unsigned int i) {
                return self[i];
            });
}
