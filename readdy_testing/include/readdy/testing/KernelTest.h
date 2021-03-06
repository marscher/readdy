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
 * @file KernelTest.h
 * @brief << brief description >>
 * @author clonker
 * @date 15.07.16
 */


#ifndef READDY_MAIN_KERNELTEST_H
#define READDY_MAIN_KERNELTEST_H

#include <gtest/gtest.h>
#include <readdy/model/Kernel.h>
#include <readdy/plugin/KernelProvider.h>

class KernelTest : public ::testing::TestWithParam<std::string> {
public:
    const readdy::plugin::KernelProvider::kernel_ptr kernel;

    explicit KernelTest() : kernel(readdy::plugin::KernelProvider::getInstance().create(GetParam())) {
    }
};

#endif //READDY_MAIN_KERNELTEST_H
