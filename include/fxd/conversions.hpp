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
#include <new> // launder()
#include <type_traits>

#include "concepts.hpp"
#include "casting.hpp"

#include "detail/bias.hpp"
#include "detail/int_to_float.hpp"
#include "detail/shift.hpp"


namespace fxd {


    template<int Int,
             int Frac,
             typename Raw>
    [[nodiscard]]
    constexpr
    fixed<Int, Frac, Raw>::operator bool()
        const noexcept
    {
        return raw_value;
    }


    /// Convert to integer.
    template<std::integral I,
             fixed_point Fxd>
    [[nodiscard]]
    constexpr
    I
    to_int(Fxd f)
        noexcept
    {
        using Raw = typename Fxd::raw_type;
        Raw raw = f.raw_value;
        if constexpr (Fxd::frac_bits >= 0) {

            if (raw < 0)
                raw += detail::make_bias_for(Fxd::frac_bits, raw);
            return detail::shr_real(raw, Fxd::frac_bits);

        } else {

            // Allow left-shifting to happen on a wider type.
            using RawWide = detail::max_int_for<Raw>;
            return detail::shl_real<RawWide>(raw, -Fxd::frac_bits);

        }
    }


    /// Convert to the "natural integer" for this fixed-point type.
    template<fixed_point Fxd>
    requires detail::has_int_for<Fxd::int_bits, typename Fxd::raw_type>
    [[nodiscard]]
    constexpr
    detail::select_int_for<Fxd::int_bits, typename Fxd::raw_type>
    to_int(Fxd f)
        noexcept
    {
        using I = detail::select_int_for<Fxd::int_bits, typename Fxd::raw_type>;
        return to_int<I>(f);
    }


    template<int Int,
             int Frac,
             typename Raw>
    template<std::integral I>
    [[nodiscard]]
    constexpr
    fixed<Int, Frac, Raw>::operator I()
        const noexcept
    {
        return to_int<I>(*this);
    }


    /// Convert a fixed-point to a floating-point type, rounds to zero.
    template<std::floating_point Flt,
             fixed_point Fxd>
    constexpr
    Flt
    to_float(Fxd f)
        noexcept
    {
        const auto r = f.raw_value;
        const Flt fraw = detail::int_to_float<Flt>(r);
        return std::ldexp(fraw, -Fxd::frac_bits);
    }


    /// Converts a fixed-point to its natural floating-point type (with no losses/rounding).
    template<fixed_point Fxd>
    [[nodiscard]]
    constexpr
    typename std::numeric_limits<Fxd>::float_type
    to_float(Fxd f)
        noexcept
    {
        return to_float<typename std::numeric_limits<Fxd>::float_type>(f);
    }


    template<int Int,
             int Frac,
             typename Raw>
    template<std::floating_point F>
    [[nodiscard]]
    constexpr
    fixed<Int, Frac, Raw>::operator F()
        const noexcept
    {
        return to_float<F>(*this);
    }


    template<int Int, int Frac, typename Raw>
    template<int Int2, int Frac2, typename Raw2>
    [[nodiscard]]
    constexpr
    fixed<Int, Frac, Raw>::operator fixed<Int2, Frac2, Raw2>()
        const noexcept
    {
        return fixed_cast<fixed<Int2, Frac2, Raw2>>(*this);
    }

}


#endif
