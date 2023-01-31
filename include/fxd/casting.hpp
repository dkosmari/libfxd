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

#include "impl/shift.hpp"


namespace fxd {


    template<fixed_point Dst,
             fixed_point Src>
    constexpr
    Dst
    fixed_cast(Src src)
        noexcept
    {
        using DstRaw = typename Dst::raw_type;
        using SrcRaw = typename Src::raw_type;
        using Raw = std::common_type_t<DstRaw, SrcRaw>;
        constexpr int diff = Dst::fractional_bits - Src::fractional_bits;
        const Raw raw = impl::shl<Raw>(src.raw_value,
                                       diff);
        return Dst::from_raw(raw);
    }


    template<int Int,
             int Frac,
             typename Raw = select_int_t<Int + Frac>,
             fixed_point Src>
    constexpr
    fixed<Int, Frac, Raw>
    fixed_cast(Src src)
        noexcept
    {
        return fixed_cast<fixed<Int, Frac, Raw>>(src);
    }



    template<int Int,
             int Frac,
             typename Raw = select_uint_t<Int + Frac>,
             fixed_point Src>
    constexpr
    fixed<Int, Frac, Raw>
    ufixed_cast(Src src)
        noexcept
    {
        return fixed_cast<fixed<Int, Frac, Raw>>(src);
    }


}


#endif
