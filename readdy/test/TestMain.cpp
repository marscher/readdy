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
// Created by clonker on 07.03.16.
//


#include <readdy/testing/Utils.h>
#include <readdy/plugin/KernelProvider.h>

#include "gtest/gtest.h"

int perform_tests(int argc, char **argv) {
    const auto dir = readdy::testing::getPluginsDirectory();
    readdy::plugin::KernelProvider::getInstance().loadKernelsFromDirectory(dir);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

int main(int argc, char **argv) {
    auto console = spdlog::stdout_color_mt("console");
    console->set_level(spdlog::level::warn);
    console->set_pattern("[          ] [%Y-%m-%d %H:%M:%S] [%t] [%l] %v");
    int result = perform_tests(argc, argv);
    spdlog::drop_all();
    return result;
}
