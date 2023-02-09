/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_MATH_HPP
#define LIBFXD_MATH_HPP

#include <cassert>
#include <cerrno>
#include <numeric>
#include <utility>

#include "concepts.hpp"
#include "constructors.hpp"
#include "limits.hpp"
#include "operators.hpp"
#include "round-div.hpp"
#include "round-mul.hpp"

#include "impl/add.hpp"


namespace fxd {

    /// Same as `std::midpoint()`
    template<fixed_point Fxd>
    constexpr
    Fxd
    midpoint(Fxd a,
             Fxd b)
        noexcept
    {
        auto mid = std::midpoint(a.raw_value, b.raw_value);
        return Fxd::from_raw(mid);
    }


    /// Same as `std::abs()`.
    template<fixed_point Fxd>
    constexpr
    Fxd
    abs(Fxd f)
        noexcept
    {
        return f.raw_value < 0 ? -f : +f;
    }


    /// Same as `std::fdim()`.
    template<fixed_point Fxd>
    constexpr
    Fxd
    fdim(Fxd a,
         Fxd b)
        noexcept
    {
        if (a > b)
            return a - b;
        else
            return 0;
    }


    /// Same as `std::nextafter()`.
    template<fixed_point Fxd>
    constexpr
    Fxd
    nextafter(Fxd from,
              Fxd to)
        noexcept
    {
        constexpr Fxd e = std::numeric_limits<Fxd>::epsilon();
        if (from < to)
            return from + e;
        if (from > to)
            return from - e;
        return to;
    }



    /**
     * @brief Same as `std::sqrt()`.
     *
     * @note If the argument `x` is negative, `errno` is set to `EDOM`.
     */
    template<fixed_point Fxd>
    requires (Fxd::int_bits > std::numeric_limits<Fxd>::is_signed
              &&
              Fxd::frac_bits >= 0)
    constexpr
    Fxd
    sqrt(Fxd x)
    {
        if constexpr (std::numeric_limits<Fxd>::is_signed) {
            if (x < 0) {
                errno = EDOM;
                return 0;
            }
        }

        if (!x)
            return x;

        // Babylonian/Heron/Newton-Raphson method

        int i = 0;
        Fxd old_b;
        Fxd a = x < 1 ? x : 1;
        Fxd b = x < 1 ? 1 : x;

        do {

            if (++i > Fxd::bits) [[unlikely]]
                break;

            a = down::div(x, b);
            assert(a >= 0);
            assert(a <= b);

            old_b = b;
            b = midpoint(b, a);
            assert(b >= 0);

            if (!b) [[unlikely]]
                break;

        } while (old_b != b);

        a = down::div(x, b);

        while (up::mul(a, a) <= x)
            ++a.raw_value;

        while (up::mul(a, a) > x)
            --a.raw_value;


        return a;
    }


}


#endif
