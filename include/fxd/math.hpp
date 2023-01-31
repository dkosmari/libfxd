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
#include "round-div.hpp"


namespace fxd {


    template<fixed_point Fxd>
    constexpr
    Fxd
    abs(Fxd f)
        noexcept
    {
        return f < 0 ? -f : f;
    }



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



    // Babylonian/Heron/Newton-Raphson method

    template<fixed_point Fxd>
    Fxd
    sqrt(Fxd x)
    {
        if constexpr (std::numeric_limits<Fxd>::is_signed) {
            if (x < 0) {
                errno = EDOM;
                return std::numeric_limits<Fxd>::quiet_NaN();
            }
        }

        if (!x)
            return x;

        int i = 0;
        Fxd a = x;
        Fxd old_a;

        do {
            if (++i > Fxd::bits)
                return a;

            Fxd b = up::div(x, a);

            old_a = a;

            a = (a + b);
            a.raw_value /= 2;

            if (!a)
                return a;

        } while (old_a != a);

        return a;
    }


}


#endif
