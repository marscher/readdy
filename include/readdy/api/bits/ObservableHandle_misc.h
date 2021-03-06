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
 * @file ObservableHandle_misc.h
 * @brief << brief description >>
 * @author clonker
 * @date 06.01.17
 * @copyright GNU Lesser General Public License v3.0
 */

#pragma once

#include "../ObservableHandle.h"

NAMESPACE_BEGIN(readdy)

inline ObservableHandle::ObservableHandle(id_t id, model::observables::ObservableBase *const observable)
        : id(id), observable(observable) {}

void
inline ObservableHandle::enableWriteToFile(readdy::io::File &file, const std::string &dataSetName, unsigned int flushStride) {
    if (observable) {
        observable->enableWriteToFile(file, dataSetName, flushStride);
    } else {
        log::warn("You just tried to enable write to file on a user-provided observable instance, "
                          "this is not supported!");
    }
}

inline ObservableHandle::id_t ObservableHandle::getId() const {
    return id;
}

inline void ObservableHandle::flush() {
    if(observable) {
        observable->flush();
    }
}

inline ObservableHandle::ObservableHandle() : ObservableHandle(0, nullptr) { }

NAMESPACE_END(readdy)
