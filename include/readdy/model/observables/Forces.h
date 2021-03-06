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
 * @file Forces.h
 * @brief << brief description >>
 * @author clonker
 * @date 13.03.17
 * @copyright GNU Lesser General Public License v3.0
 */

#pragma once

#include <readdy/common/macros.h>
#include <readdy/model/Vec3.h>
#include "Observable.h"

NAMESPACE_BEGIN(readdy)
NAMESPACE_BEGIN(model)
NAMESPACE_BEGIN(observables)

class Forces : public Observable<std::vector<readdy::model::Vec3>> {

public:
    Forces(Kernel *const kernel, unsigned int stride);

    Forces(Kernel *const kernel, unsigned int stride, std::vector<std::string> typesToCount);

    Forces(Kernel *const kernel, unsigned int stride, std::vector<unsigned int> typesToCount);

    virtual ~Forces();

    void flush() override;

protected:
    void initializeDataSet(io::File &file, const std::string &dataSetName, unsigned int flushStride) override;

    void append() override;

    struct Impl;
    std::unique_ptr<Impl> pimpl;

    std::vector<unsigned int> typesToCount;
};
NAMESPACE_END(observables)
NAMESPACE_END(model)
NAMESPACE_END(readdy)