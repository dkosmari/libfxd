/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_MATH_HPP
#define LIBFXD_MATH_HPP

#include <cerrno>

#include "concepts.hpp"
#include "limits.hpp"
#include "operators.hpp"
#include "round-div.hpp"


namespace fxd {

    /// Same as `std::abs()`.
    template<fixed_point Fxd>
    constexpr
    Fxd
    abs(Fxd f)
        noexcept
    {
        return f < 0 ? -f : f;
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
        Fxd a = x;
        Fxd old_a;

        do {

            if (++i > Fxd::bits) [[unlikely]]
                break;

            Fxd b = up::div(x, a);

            old_a = a;

            a = (a + b);
            a.raw_value >>= 1;

            if (!a) [[unlikely]]
                break;

        } while (old_a != a);

        while (a * a > x)
            --a.raw_value;

        return a;
    }


}


#endif
