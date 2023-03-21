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
        using DstRaw = typename Dst::raw_type;

        SrcRaw src_raw = src.raw_value;
        DstRaw dst_raw;

        constexpr int diff = Dst::frac_bits - Src::frac_bits;

        if constexpr (diff < 0) {

            // shifting right
            if (src_raw < 0) // if negative, add a bias so it rounds up
                src_raw += detail::make_bias_for(-diff, src_raw);

            // note: we shift using SrcRaw type, to not lose high bits early
            dst_raw = detail::shr_real(src_raw, -diff);

        } else {

            // shifting left, no rounding happens
            // note: we shift using DstRaw type, in case it has more bits than SrcRaw
            dst_raw = detail::shl_real<DstRaw>(src_raw, diff);

        }

        return Dst::from_raw(dst_raw);
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



    // TODO: add rounding versions


}


#endif
