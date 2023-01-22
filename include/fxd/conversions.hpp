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


    template<int Int,
             int Frac,
             typename Raw>
    template<std::integral I>
    constexpr
    fixed<Int, Frac, Raw>::operator I()
        const noexcept
    {
        if constexpr (frac_bits < 0) {
            return utils::shift::shl_real(raw_value, -frac_bits);
        } else {
            raw_type v = raw_value;
            if (v < 0)
                v += utils::shift::make_bias_for(frac_bits, v);

            return utils::shift::shr_real(v, frac_bits);
        }
    }


    template<int Int,
             int Frac,
             typename Raw>
    template<std::floating_point Flt>
    constexpr
    fixed<Int, Frac, Raw>::operator Flt()
        const noexcept
    {
        return std::ldexp(static_cast<Flt>(utils::opacify(raw_value)), -frac_bits);
    }


    template<fixed_point Fxd>
    requires(!std::is_void_v<typename Fxd::float_type>)
    constexpr
    typename Fxd::float_type
    to_float(Fxd f)
        noexcept
    {
        using Flt = typename Fxd::float_type;
        static_assert(!std::is_void_v<Flt>);
        return static_cast<Flt>(f);
    }

}


#endif
