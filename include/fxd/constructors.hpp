/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_CONSTRUCTORS_HPP
#define LIBFXD_CONSTRUCTORS_HPP

#include <cfenv>
#include <cmath>
#include <concepts>
#include <type_traits>

#include "fixed.hpp"

#include "impl/shift.hpp"


namespace fxd {


    template<int Int,
             int Frac,
             typename Raw>
    template<std::integral I>
    constexpr
    fixed<Int, Frac, Raw>::fixed(I i)
        noexcept :
        raw_value{impl::shl<raw_type>(i, frac_bits)}
    {}


    template<int Int,
             int Frac,
             typename Raw>
    template<std::floating_point Flt>
    constexpr
    fixed<Int, Frac, Raw>::fixed(Flt f)
        noexcept
    {
        const auto scaled_f = std::ldexp(f, frac_bits);
        if (std::is_constant_evaluated())
            raw_value = static_cast<raw_type>(scaled_f);
        else
            raw_value = static_cast<raw_type>(std::rint(scaled_f));
    }


    template<int Int,
             int Frac,
             typename Raw>
    constexpr
    fixed<Int, Frac, Raw>
    fixed<Int, Frac, Raw>::from_raw(Raw val)
        noexcept
    {
        fixed result;
        result.raw_value = val;
        return result;
    }


}


#endif
