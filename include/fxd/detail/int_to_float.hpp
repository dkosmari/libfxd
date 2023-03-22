/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_DETAIL_INT_TO_FLOAT_HPP
#define LIBFXD_DETAIL_INT_TO_FLOAT_HPP

#include <bit> // bit_width()
#include <concepts>
#include <limits>
#include <type_traits> // make_unsigned_t


namespace fxd::detail {

    // This converts an integer to a floating-point type, ensuring it
    // rounds to zero.
    template<std::floating_point Flt,
             std::unsigned_integral U>
    constexpr
    Flt
    int_to_float(U u)
        noexcept
    {
        if constexpr (std::numeric_limits<Flt>::digits
                      >=
                      std::numeric_limits<U>::digits) {

            return static_cast<Flt>(u);

        } else {

            const int uw = std::bit_width(u);
            constexpr int fw = std::numeric_limits<Flt>::digits;
            if (uw <= fw) // u fits with no rounding
                return static_cast<Flt>(u);

            // otherwise, we must discard the lower bits of u until it fits
            const int diff = uw - fw;
            // these shifts clear out the lower bits, so no rounding happens
            return static_cast<Flt>((u >> diff) << diff);

        }

    }


    template<std::floating_point Flt,
             std::signed_integral S>
    constexpr
    Flt
    int_to_float(S s)
        noexcept
    {
        using U = std::make_unsigned_t<S>;

        if (s >= 0)
            return int_to_float<Flt, U>(s);

        if (s == std::numeric_limits<S>::min())
            // it's a power-of-two, so it never rounds
            return static_cast<Flt>(s);

        return -int_to_float<Flt, U>(-s);
    }

}

#endif
