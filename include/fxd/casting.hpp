/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_CASTING_HPP
#define LIBFXD_CASTING_HPP

#include <type_traits>

#include "concepts.hpp"

#include "detail/bias.hpp"
#include "detail/shift.hpp"


namespace fxd {


    /// Convert a fixed point to a different type of fixed point.
    template<fixed_point Dst,
             fixed_point Src>
    [[nodiscard]]
    constexpr
    Dst
    fixed_cast(Src src)
        noexcept
    {
        using SrcRaw = typename Src::raw_type;
        SrcRaw sraw = src.raw_value;

        constexpr int diff = Dst::frac_bits - Src::frac_bits;

        if constexpr (diff < 0) {

            // shifting right
            if (sraw < 0)
                sraw += detail::make_bias_for(-diff, sraw);

            auto draw = detail::shr_real(sraw, -diff);
            return Dst::from_raw(draw);

        } else {

            // shifting left
            // use more bits
            using SrcWide = detail::max_int_for<SrcRaw>;
            auto draw = detail::shl_real<SrcWide>(sraw, diff);
            return Dst::from_raw(draw);

        }
    }


    /// Convert a fixed point to a different type of fixed point.
    template<int Int,
             int Frac,
             typename Raw = detail::select_int_t<Int + Frac>,
             fixed_point Src>
    [[nodiscard]]
    constexpr
    fixed<Int, Frac, Raw>
    fixed_cast(Src src)
        noexcept
    {
        return fixed_cast<fixed<Int, Frac, Raw>>(src);
    }


    /// Convert a fixed point to a different type of fixed point (unsigned version.)
    template<int Int,
             int Frac,
             typename Raw = detail::select_uint_t<Int + Frac>,
             fixed_point Src>
    [[nodiscard]]
    constexpr
    fixed<Int, Frac, Raw>
    ufixed_cast(Src src)
        noexcept
    {
        return fixed_cast<fixed<Int, Frac, Raw>>(src);
    }


}


#endif
