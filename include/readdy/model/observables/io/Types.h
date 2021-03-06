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
 * @file Types.h
 * @brief << brief description >>
 * @author clonker
 * @date 13.03.17
 * @copyright GNU Lesser General Public License v3.0
 */

#pragma once

#include <readdy/common/macros.h>
#include <readdy/io/DataSetType.h>
#include <readdy/model/Vec3.h>
#include <readdy/model/reactions/ReactionRecord.h>
#include "TrajectoryEntry.h"

NAMESPACE_BEGIN(readdy)
NAMESPACE_BEGIN(model)
NAMESPACE_BEGIN(observables)
NAMESPACE_BEGIN(util)

constexpr auto OBSERVABLES_GROUP_PATH = "/readdy/observables";

class Vec3MemoryType : public readdy::io::DataSetType {
    static io::NativeCompoundType get() {
        using entry_t = Vec3;

        static readdy::io::NativeCompoundType type = readdy::io::NativeCompoundTypeBuilder(sizeof(entry_t))
                .insert<decltype(std::declval<entry_t>().x)>("x", offsetof(entry_t, x))
                .insert<decltype(std::declval<entry_t>().y)>("y", offsetof(entry_t, y))
                .insert<decltype(std::declval<entry_t>().z)>("z", offsetof(entry_t, z))
                .build();
        return type;
    }

public:
    Vec3MemoryType() : DataSetType(-1) {
        using entry_t = Vec3;
        hid_t entryTypeMemory = H5Tcreate(H5T_COMPOUND, sizeof(entry_t));
        // init vec pod
        readdy::io::NativeDataSetType<entry_t::value_t> posType{};
        H5Tinsert(entryTypeMemory, "x", HOFFSET(entry_t, x), posType.hid());
        H5Tinsert(entryTypeMemory, "y", HOFFSET(entry_t, y), posType.hid());
        H5Tinsert(entryTypeMemory, "z", HOFFSET(entry_t, z), posType.hid());

        handle->set(entryTypeMemory);
    }
};

class Vec3FileType : public readdy::io::DataSetType {
public:
    Vec3FileType() : DataSetType(-1) {
        using entry_t = Vec3;
        auto tid = []() -> hid_t {
            Vec3MemoryType memType{};
            auto file_type = H5Tcopy(memType.hid());
            H5Tpack(file_type);
            return file_type;
        }();
        handle->set(tid);
    }
};

class ReactionRecordPODMemoryType : public readdy::io::DataSetType {
    using entry_t = readdy::model::reactions::ReactionRecord;

    io::NativeCompoundType compoundType;
public:
    ReactionRecordPODMemoryType() :
            DataSetType(-1),
            compoundType(io::NativeCompoundTypeBuilder(sizeof(entry_t))
                                   .insert<decltype(std::declval<entry_t>().type)>("reaction_type", offsetof(entry_t, type))
                                   .insertStdArray<decltype(std::declval<entry_t>().educts)>("educts", offsetof(entry_t, educts))
                                   .insertStdArray<decltype(std::declval<entry_t>().products)>("products", offsetof(entry_t, products))
                                   .insertArray<Vec3::value_t, 3>("position", offsetof(entry_t, where))
                                   .insertStdArray<decltype(std::declval<entry_t>().types_from)>("types_from", offsetof(entry_t, types_from))
                                   .insert<decltype(std::declval<entry_t>().reactionIndex)>("reaction_index", offsetof(entry_t, reactionIndex))
                                   .build()) {
    }

    virtual io::h5::handle_t hid() const override {
        return compoundType.hid();
    }
};

class ReactionRecordPODFileType : public readdy::io::DataSetType {
public:
    ReactionRecordPODFileType(): DataSetType(-1) {
        using entry_t = readdy::model::reactions::ReactionRecord;
        ReactionRecordPODMemoryType memType{};
        auto file_type = H5Tcopy(memType.hid());
        H5Tpack(file_type);
        handle->set(file_type);
    }
};

class TrajectoryEntryMemoryType : public readdy::io::NativeCompoundType {
    static readdy::io::NativeCompoundType get() {
        using entry_t = readdy::model::observables::TrajectoryEntry;

        static readdy::io::NativeCompoundType type = readdy::io::NativeCompoundTypeBuilder(sizeof(entry_t))
                .insert<decltype(std::declval<entry_t>().id)>("id", offsetof(entry_t, id))
                .insert<decltype(std::declval<entry_t>().typeId)>("typeId", offsetof(entry_t, typeId))
                .insert<decltype(std::declval<entry_t>().flavor)>("flavor", offsetof(entry_t, flavor))
                .insertArray<Vec3::value_t, 3>("pos", offsetof(entry_t, pos))
                .build();
        return type;
    }

public:

    TrajectoryEntryMemoryType() : NativeCompoundType(get()) {}
};

class TrajectoryEntryFileType : public readdy::io::STDCompoundType {
public:
    TrajectoryEntryFileType() : STDCompoundType(TrajectoryEntryMemoryType()) {
    }
};

NAMESPACE_END(util)
NAMESPACE_END(observables)
NAMESPACE_END(model)
NAMESPACE_END(readdy)
