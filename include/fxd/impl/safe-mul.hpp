/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_INCLUDING_IMPL_SAFE_HPP
#error "Do not include this header directly. Either include 'saturate.hpp' or 'except.hpp'."
#endif

// Range-checked rounding to zero.
namespace zero {

    /// Multiply rounding to zero, when no fractional bits, with range check.
    template<fixed_point Fxd>
    requires (Fxd::frac_bits <= 0)
        constexpr
        Fxd
        mul(Fxd a,
            Fxd b)
    {
        constexpr int w = impl::type_width<typename Fxd::raw_type>;
        // offset used for shifting left.
        constexpr int offset = w - Fxd::frac_bits;
        const auto c = impl::mul<Fxd::bits>(a.raw_value, b.raw_value);

        // no lower bits will be discarded, so no rounding needed
        const auto d = impl::shl_real(c, offset);

        // unshift, check that the value matches
        if (offset > 0 && c != impl::shr_real(d, offset))
            return handler<Fxd>(impl::is_negative(c)
                                ? impl::error::underflow
                                : impl::error::overflow);

        return from_raw<Fxd>(impl::last(d));
    }



    /// Multiply rounding to zero, when there are fractional bits, with range check.
    template<fixed_point Fxd>
    requires (Fxd::frac_bits > 0)
        constexpr
        Fxd
        mul(Fxd a,
            Fxd b)
    {
        constexpr int w = impl::type_width<typename Fxd::raw_type>;
        // offset used for shifting left.
        constexpr int offset = w - Fxd::frac_bits;

        auto c = impl::mul<Fxd::bits>(a.raw_value, b.raw_value);

        if (impl::is_negative(c)) {
            // negative numbers need a bias to zero when shifting
            const auto bias = impl::make_bias_for(Fxd::frac_bits, c);
            c = impl::add(c, bias);
        }

        const auto d = impl::shl(c, offset);
        // unshift, check that the value matches
        if (offset > 0 && c != impl::shr(d, offset))
            return handler<Fxd>(impl::is_negative(c)
                                ? impl::error::underflow
                                : impl::error::overflow);

        return from_raw<Fxd>(impl::last(d));
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
        constexpr int w = impl::type_width<typename Fxd::raw_type>;

        // offset used for shifting left
        constexpr int offset = w - Fxd::frac_bits;

        const auto c = impl::mul<Fxd::bits>(a.raw_value, b.raw_value);

        if constexpr (Fxd::frac_bits <= 0) {
            // offset >= w

            const auto [d, ovf] = impl::overflow::shl_real(c, offset);
            if (ovf)
                return handler<Fxd>(impl::is_negative(c)
                                    ? impl::error::underflow
                                    : impl::error::overflow);

            return from_raw<Fxd>(impl::last(d));

        } else {
            // offset < w

            const auto bias = impl::make_bias_for(Fxd::frac_bits, c);
            const auto [biased_c, ovf] = impl::overflow::add(c, bias);
            if (ovf)
                return handler<Fxd>(impl::error::overflow);

            const auto [d, ovf2] = impl::overflow::shl(biased_c, offset);
            if (offset > 0 && ovf2)
                return handler<Fxd>(impl::is_negative(c)
                                    ? impl::error::underflow
                                    : impl::error::overflow);

            return from_raw<Fxd>(impl::last(d));
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
        constexpr int w = impl::type_width<typename Fxd::raw_type>;

        // offset used for shifting left
        constexpr int offset = w - Fxd::frac_bits;

        const auto c = impl::mul<Fxd::bits>(a.raw_value, b.raw_value);

        const auto [d, ovf] = impl::overflow::shl(c, offset);
        if (offset > 0 && ovf)
            return handler<Fxd>(impl::is_negative(c)
                                ? impl::error::underflow
                                : impl::error::overflow);

        return from_raw<Fxd>(impl::last(d));
    }

} // namespace down
