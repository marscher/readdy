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
 * This header file contains the definition of ObservableWrapper. It wraps a function object of the form
 * std::function<void(readdy::model::time_step_type)> together with a pointer to the kernel and a stride attribute
 * into an observable.
 *
 * @file ObservableWrapper.h
 * @brief This header file contains the definition of ObservableWrapper, which wraps a std::function to an observable.
 * @author clonker
 * @date 10.05.16
 */

#pragma once

#include <readdy/model/observables/Observable.h>

NAMESPACE_BEGIN(readdy)
NAMESPACE_BEGIN(model)
NAMESPACE_BEGIN(observables)

class ObservableWrapper : public ObservableBase {
public:
    ObservableWrapper(Kernel *const kernel,
                      const observables::observable_type &observable, unsigned int stride = 1);

    void operator()(time_step_type t);

    virtual void evaluate() override;

    void flush() override;

protected:
    void initializeDataSet(io::File &file, const std::string &dataSetName, unsigned int flushStride) override;

    void append() override;

    const observables::observable_type observable;
};

NAMESPACE_END(observables)
NAMESPACE_END(model)
NAMESPACE_END(readdy)
