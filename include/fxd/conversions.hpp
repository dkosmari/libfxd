/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_CONVERSIONS_HPP
#define LIBFXD_CONVERSIONS_HPP

#include <cmath>
#include <concepts>
#include <type_traits>

#include "concepts.hpp"
#include "utils-shift.hpp"
#include "utils.hpp"


namespace fxd {


    template<int Int,
             int Frac,
             typename Raw>
    constexpr
    fixed<Int, Frac, Raw>::operator bool()
        const noexcept
    {
        return raw_value;
    }



    template<std::integral I,
             fixed_point Fxd>
    constexpr
    I
    to_int(Fxd f)
        noexcept
    {
        using Raw = typename Fxd::raw_type;
        Raw raw = f.raw_value;
        if constexpr (Fxd::frac_bits > 0) {
            if (raw < 0)
                raw += utils::shift::make_bias_for(Fxd::frac_bits, raw);
            return utils::shift::shr_real(raw, Fxd::frac_bits);
        } else {
            // Allow left-shifting to happen on a wider type
            using Common = std::common_type_t<Raw, I>;
            return utils::shift::shl_real<Common>(raw, -Fxd::frac_bits);
        }
    }


    template<int Int,
             int Frac,
             typename Raw>
    template<std::integral I>
    constexpr
    fixed<Int, Frac, Raw>::operator I()
        const noexcept
    {
        return to_int<I>(*this);
    }





    template<std::floating_point Flt,
             fixed_point Fxd>
    constexpr
    Flt
    to_float(Fxd f)
        noexcept
    {
        const Flt fraw = static_cast<Flt>(utils::opacify(f.raw_value));
        return std::ldexp(fraw, -Fxd::frac_bits);
    }


    template<fixed_point Fxd>
    constexpr
    typename Fxd::float_type
    to_float(Fxd f)
        noexcept
    {
        return to_float<typename Fxd::float_type>(f);
    }


    template<int Int,
             int Frac,
             typename Raw>
    template<std::floating_point Flt>
    constexpr
    fixed<Int, Frac, Raw>::operator Flt()
        const noexcept
    {
        return to_float<Flt>(*this);
    }


}


#endif
