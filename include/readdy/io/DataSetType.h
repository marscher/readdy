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
 * @file DataSetType.h
 * @brief << brief description >>
 * @author clonker
 * @date 04/01/2017
 * @copyright GNU Lesser General Public License v3.0
 */
#ifndef READDY_MAIN_DATASETTYPE_H
#define READDY_MAIN_DATASETTYPE_H

#include <vector>
#include <readdy/common/logging.h>
#include <readdy/common/traits.h>

namespace readdy {
namespace io {

class DataTypeHandle {
public:
    h5::data_set_type_t tid;
    DataTypeHandle(h5::data_set_type_t tid) : tid(tid) {}

    virtual ~DataTypeHandle() {
        if(tid >= 0) {
            if(H5Tclose(tid) < 0) {
                log::error("error on closing data set type");
                H5Eprint (H5Eget_current_stack(), stderr);
            }
        }
    }
};

class READDY_API DataSetType {
public:
    std::shared_ptr<DataTypeHandle> tid;
};

template<typename T>
class READDY_API NativeDataSetType : public DataSetType {
public:
    NativeDataSetType();
    using type = T;
};

template<typename T, unsigned int len>
class READDY_API NativeArrayDataSetType : public DataSetType {
public:
    using type = typename std::remove_pointer<typename std::decay<T>::type>::type;
    NativeArrayDataSetType();
    constexpr static unsigned int size = len;
private:
    NativeDataSetType<type> nativeType;
};

template<typename T, typename = typename std::enable_if<util::is_std_array<T>::value>::type>
class READDY_API NativeStdArrayDataSetType : public DataSetType {
public:
    using type = typename T::value_type;
    NativeStdArrayDataSetType();
    constexpr static std::size_t size = std::tuple_size<T>::value;
private:
    NativeDataSetType<type> nativeType;
};

template<typename T>
class READDY_API STDDataSetType : public DataSetType {
public:
    STDDataSetType();
    using type = T;
};

template<typename T, unsigned int len>
class READDY_API STDArrayDataSetType : public DataSetType {
public:
    using type = typename std::remove_pointer<typename std::decay<T>::type>::type;
    STDArrayDataSetType();
    constexpr static unsigned int size = len;
private:
    STDDataSetType<type> stdType;
};

class NativeCompoundTypeBuilder;
class READDY_API NativeCompoundType : public DataSetType {
    friend class readdy::io::NativeCompoundTypeBuilder;
    NativeCompoundType(h5::data_set_type_t tid);
};

class NativeCompoundTypeBuilder {
public:
    NativeCompoundTypeBuilder(std::size_t size);
    NativeCompoundTypeBuilder& insert(const std::string& name, std::size_t offset, h5::data_set_type_t type);
    template<typename T>
    NativeCompoundTypeBuilder& insert(const std::string& name, std::size_t offset);
    template<typename T, unsigned int size>
    NativeCompoundTypeBuilder& insertArray(const std::string&name, std::size_t offset);
    template<typename T, typename = typename std::enable_if<util::is_std_array<T>::value>::type>
    NativeCompoundTypeBuilder& insertStdArray(const std::string &name, std::size_t offset);
    NativeCompoundType build();

private:
    h5::data_set_type_t tid;
};

class READDY_API STDCompoundType : public DataSetType {
public:
    STDCompoundType(const NativeCompoundType& nativeType);
};


}
}

#include "bits/DataSetType_bits.h"

#endif //READDY_MAIN_DATASETTYPE_H
