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
#include "impl/bias.hpp"
#include "impl/opacify.hpp"
#include "impl/shift.hpp"


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
                raw += impl::make_bias_for(Fxd::frac_bits, raw);
            return impl::shr_real(raw, Fxd::frac_bits);

        } else {

            // Allow left-shifting to happen on a wider type.
            using RawWide = impl::max_int_for<Raw>;
            return impl::shl_real<RawWide>(raw, -Fxd::frac_bits);

        }
    }


    /// Convert to the "natural integer" for this fixed-point type.
    template<fixed_point Fxd>
    requires impl::has_int_for<Fxd::int_bits, typename Fxd::raw_type>
    [[nodiscard]]
    constexpr
    impl::select_int_for<Fxd::int_bits, typename Fxd::raw_type>
    to_int(Fxd f)
        noexcept
    {
        using I = impl::select_int_for<Fxd::int_bits, typename Fxd::raw_type>;
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


    /// Convert a fixed-point to a floating-point type.
    template<std::floating_point Flt,
             fixed_point Fxd>
    constexpr
    Flt
    to_float(Fxd f)
        noexcept
    {
#if defined(__clang__) && __clang_major__ >= 12 && !defined(__FAST_MATH__)
#pragma STDC FENV_ACCESS ON
#endif

        const auto r = f.raw_value;
#if 0
        // Note: all compilers fail to respect rounding mode, so this isn't safe.
        const Flt fraw = static_cast<Flt>(r);
#else
        // This version kills optimizations to ensure correct code.
        const Flt fraw = static_cast<Flt>(impl::opacify(r));
#endif

        return std::ldexp(fraw, -Fxd::frac_bits);
    }


    /// Converts a fixed-point to its natural floating-point type (with no losses/rounding).
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
    template<std::floating_point F>
    constexpr
    fixed<Int, Frac, Raw>::operator F()
        const noexcept
    {
        return to_float<F>(*this);
    }


}


#endif
