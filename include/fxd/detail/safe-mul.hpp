/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_INCLUDING_DETAIL_SAFE_HPP
#error "Do not include this header directly. Either include 'saturate.hpp' or 'except.hpp'."
#endif

namespace zero {

    // Multiply rounding to zero, when no fractional bits, with range check.
    template<fixed_point Fxd>
    requires (Fxd::frac_bits <= 0)
        constexpr
        Fxd
        mul(Fxd a,
            Fxd b)
    {
        constexpr int w = detail::type_width<typename Fxd::raw_type>;
        // offset used for shifting left.
        constexpr int offset = w - Fxd::frac_bits;
        const auto c = detail::mul<Fxd::bits>(a.raw_value, b.raw_value);

        // no lower bits will be discarded, so no rounding needed
        const auto d = detail::shl_real(c, offset);

        // unshift, check that the value matches
        if (offset > 0 && c != detail::shr_real(d, offset))
            return handler<Fxd>(detail::is_negative(c)
                                ? detail::error::underflow
                                : detail::error::overflow);

        return from_raw<Fxd>(detail::last(d));
    }



    // Multiply rounding to zero, when there are fractional bits, with range check.
    template<fixed_point Fxd>
    requires (Fxd::frac_bits > 0)
        constexpr
        Fxd
        mul(Fxd a,
            Fxd b)
    {
        constexpr int w = detail::type_width<typename Fxd::raw_type>;
        // offset used for shifting left.
        constexpr int offset = w - Fxd::frac_bits;

        auto c = detail::mul<Fxd::bits>(a.raw_value, b.raw_value);

        if (detail::is_negative(c)) {
            // negative numbers need a bias to zero when shifting
            const auto bias = detail::make_bias_for(Fxd::frac_bits, c);
            c = detail::add(c, bias);
        }

        const auto d = detail::shl(c, offset);
        // unshift, check that the value matches
        if (offset > 0 && c != detail::shr(d, offset))
            return handler<Fxd>(detail::is_negative(c)
                                ? detail::error::underflow
                                : detail::error::overflow);

        return from_raw<Fxd>(detail::last(d));
    }


} // namespace zero


// Range-checked rounding up.
namespace up {

    template<fixed_point Fxd>
    constexpr
    Fxd
    mul(Fxd a,
        Fxd b)
    {
        constexpr int w = detail::type_width<typename Fxd::raw_type>;

        // offset used for shifting left
        constexpr int offset = w - Fxd::frac_bits;

        const auto c = detail::mul<Fxd::bits>(a.raw_value, b.raw_value);

        if constexpr (Fxd::frac_bits <= 0) {
            // offset >= w

            const auto [d, ovf] = detail::overflow::shl_real(c, offset);
            if (ovf)
                return handler<Fxd>(detail::is_negative(c)
                                    ? detail::error::underflow
                                    : detail::error::overflow);

            return from_raw<Fxd>(detail::last(d));

        } else {
            // offset < w

            const auto bias = detail::make_bias_for(Fxd::frac_bits, c);
            const auto [biased_c, ovf] = detail::overflow::add(c, bias);
            if (ovf)
                return handler<Fxd>(detail::error::overflow);

            const auto [d, ovf2] = detail::overflow::shl(biased_c, offset);
            if (offset > 0 && ovf2)
                return handler<Fxd>(detail::is_negative(c)
                                    ? detail::error::underflow
                                    : detail::error::overflow);

            return from_raw<Fxd>(detail::last(d));
        }

    }


} // namespace up



namespace down {

    template<fixed_point Fxd>
    constexpr
    Fxd
    mul(Fxd a,
        Fxd b)
    {
        constexpr int w = detail::type_width<typename Fxd::raw_type>;

        // offset used for shifting left
        constexpr int offset = w - Fxd::frac_bits;

        const auto c = detail::mul<Fxd::bits>(a.raw_value, b.raw_value);

        const auto [d, ovf] = detail::overflow::shl(c, offset);
        if (offset > 0 && ovf)
            return handler<Fxd>(detail::is_negative(c)
                                ? detail::error::underflow
                                : detail::error::overflow);

        return from_raw<Fxd>(detail::last(d));
    }

} // namespace down
