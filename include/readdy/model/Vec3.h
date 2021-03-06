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
 * A three-dimensional vector.
 *
 * @file Vec3.h
 * @brief File containing the definitions for Vec3.
 * @author clonker
 * @date 21.04.16
 */

#pragma once
#include <array>
#include <math.h>
#include <readdy/common/common.h>

NAMESPACE_BEGIN(readdy)
NAMESPACE_BEGIN(model)

struct READDY_API Vec3 {
public:

    using value_t = scalar;
    using data_t = std::array<value_t, 3>;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
    union {
        struct { value_t x, y, z; };
        data_t data;
    };
#pragma GCC diagnostic pop


    Vec3();

    Vec3(value_t x, value_t y, value_t z);

    Vec3(const data_t &xyz);

    Vec3 &operator+=(const Vec3 &rhs);

    Vec3 &operator-=(const Vec3 &rhs);

    Vec3 &operator*=(const value_t a);

    Vec3 &operator/=(const value_t a);

    Vec3 cross(const Vec3&) const;

    double norm() const;

    double normSquared() const;

    value_t operator[](const unsigned int i) const;

    value_t &operator[](const unsigned int i);

    Vec3& invertElementWise();

    bool operator==(const Vec3 &rhs) const;

    bool operator!=(const Vec3 &rhs) const;

    friend std::ostream &operator<<(std::ostream &, const Vec3 &);

    friend Vec3 operator+(const Vec3 &lhs, const Vec3 &rhs);

    friend Vec3 operator+(const Vec3 &lhs, const value_t rhs);

    friend Vec3 operator-(const Vec3 &lhs, const Vec3 &rhs);

    friend Vec3 operator-(const Vec3 &lhs, const value_t rhs);

    friend Vec3 operator/(const Vec3 &lhs, const value_t rhs);

    friend bool operator>=(const Vec3 &lhs, const Vec3 &rhs);

    friend bool operator<=(const Vec3 &lhs, const Vec3 &rhs);

    friend bool operator>(const Vec3 &lhs, const Vec3 &rhs);

    friend bool operator<(const Vec3 &lhs, const Vec3 &rhs);

};

inline Vec3::value_t operator*(const Vec3 &lhs, const Vec3 &rhs) {
    return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2];
}

inline Vec3 operator*(const Vec3 &lhs, const Vec3::value_t rhs) {
    return Vec3(rhs * lhs[0], rhs * lhs[1], rhs * lhs[2]);
}

inline Vec3 operator*(const Vec3::value_t rhs, const Vec3 &lhs) {
    return lhs * rhs;
}

template<bool PX, bool PY, bool PZ>
inline void fixPosition(Vec3 &vec, const Vec3::value_t dx, const Vec3::value_t dy, const Vec3::value_t dz) {
    if (PX) {
        vec[0] -= floor((vec[0] + .5 * dx) / dx) * dx;
    }
    if (PY) {
        vec[1] -= floor((vec[1] + .5 * dy) / dy) * dy;
    }
    if (PZ) {
        vec[2] -= floor((vec[2] + .5 * dz) / dz) * dz;
    }
}

template<bool PX, bool PY, bool PZ>
inline Vec3 applyPBC(Vec3 in, const Vec3::value_t dx, const Vec3::value_t dy, const Vec3::value_t dz);

template<>
inline Vec3 applyPBC<true, false, false> (Vec3 in, const Vec3::value_t dx, const Vec3::value_t dy, const Vec3::value_t dz) {
    return {in[0] - floor((in[0] + .5 * dx) / dx) * dx, in[1], in[2]};
}

template<>
inline Vec3 applyPBC<false, true, false> (Vec3 in, const Vec3::value_t dx, const Vec3::value_t dy, const Vec3::value_t dz) {
    return {in[0], in[1] - floor((in[1] + .5 * dy) / dy) * dy, in[2]};
}

template<>
inline Vec3 applyPBC<false, false, true> (Vec3 in, const Vec3::value_t dx, const Vec3::value_t dy, const Vec3::value_t dz) {
    return {in[0], in[1], in[2] - floor((in[2] + .5 * dz) / dz) * dz};
}

template<>
inline Vec3 applyPBC<true, true, false> (Vec3 in, const Vec3::value_t dx, const Vec3::value_t dy, const Vec3::value_t dz) {
    return {in[0] - floor((in[0] + .5 * dx) / dx) * dx, in[1] - floor((in[1] + .5 * dy) / dy) * dy, in[2]};
}

template<>
inline Vec3 applyPBC<true, false, true> (Vec3 in, const Vec3::value_t dx, const Vec3::value_t dy, const Vec3::value_t dz) {
    return {in[0] - floor((in[0] + .5 * dx) / dx) * dx, in[1], in[2] - floor((in[2] + .5 * dz) / dz) * dz};
}

template<>
inline Vec3 applyPBC<false, true, true> (Vec3 in, const Vec3::value_t dx, const Vec3::value_t dy, const Vec3::value_t dz) {
    return {in[0], in[1] - floor((in[1] + .5 * dy) / dy) * dy, in[2] - floor((in[2] + .5 * dz) / dz) * dz};
}

template<>
inline Vec3 applyPBC<true, true, true> (Vec3 in, const Vec3::value_t dx, const Vec3::value_t dy, const Vec3::value_t dz) {
    return {in[0] - floor((in[0] + .5 * dx) / dx) * dx,
            in[1] - floor((in[1] + .5 * dy) / dy) * dy,
            in[2] - floor((in[2] + .5 * dz) / dz) * dz};
}

template<>
inline Vec3 applyPBC<false, false, false> (Vec3 in, const Vec3::value_t dx, const Vec3::value_t dy, const Vec3::value_t dz) {
    return in;
}

template<bool PX, bool PY, bool PZ>
inline Vec3 shortestDifference(const Vec3 &lhs, const Vec3 &rhs, const Vec3::value_t dx, const Vec3::value_t dy,
                               const Vec3::value_t dz) {
    auto dv = rhs - lhs;
    if (PX) {
        if (dv[0] > dx * .5) dv[0] -= dx;
        else if (dv[0] <= -dx * .5) dv[0] += dx;
    }
    if (PY) {
        if (dv[1] > dy * .5) dv[1] -= dy;
        else if (dv[1] <= -dy * .5) dv[1] += dy;
    }
    if (PZ) {
        if (dv[2] > dz * .5) dv[2] -= dz;
        else if (dv[2] <= -dz * .5) dv[2] += dz;
    }
    return dv;
}

template<bool PX, bool PY, bool PZ>
inline Vec3::value_t
distSquared(const Vec3 &lhs, const Vec3 &rhs, const Vec3::value_t dx, const Vec3::value_t dy, const Vec3::value_t dz) {
    auto dv = shortestDifference<PX, PY, PZ>(lhs, rhs, dx, dy, dz);
    return dv * dv;
}

NAMESPACE_END(model)
NAMESPACE_END(readdy)
