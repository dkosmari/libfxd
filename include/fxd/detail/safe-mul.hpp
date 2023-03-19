/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_DETAIL_SAFE_MUL_HPP
#define LIBFXD_DETAIL_SAFE_MUL_HPP

#include <limits>

#include "../concepts.hpp"

#include "add.hpp"
#include "bias.hpp"
#include "raw-mul.hpp"
#include "shift.hpp"
#include "tuple.hpp"
#include "types.hpp"


namespace fxd::detail::safe {


    namespace down {

        template<fixed_point Fxd>
        [[nodiscard]]
        constexpr
        expected<Fxd, error>
        mul(Fxd a,
            Fxd b)
            noexcept
        {
            constexpr int w = type_width<typename Fxd::raw_type>;

            // offset used for shifting left
            constexpr int offset = w - Fxd::frac_bits;

            const auto raw_a = a.raw_value;
            const auto raw_b = b.raw_value;
            const auto raw_c = raw::mul<Fxd::bits>(raw_a, raw_b);

            const auto [raw_d, ovf] = overflow::shl(raw_c, offset);
            if (offset > 0 && ovf)
                return unexpected{is_negative(raw_c) ? error::underflow : error::overflow};

            return safe::from_raw<Fxd>(high(raw_d));
        }

    } // namespace down


    // Range-checked rounding up.
    namespace up {

        template<fixed_point Fxd>
        [[nodiscard]]
        constexpr
        expected<Fxd, error>
        mul(Fxd a,
            Fxd b)
            noexcept
        {
            constexpr int w = type_width<typename Fxd::raw_type>;

            // offset used for shifting left
            constexpr int offset = w - Fxd::frac_bits;

            const auto raw_a = a.raw_value;
            const auto raw_b = b.raw_value;
            const auto raw_c = raw::mul<Fxd::bits>(raw_a, raw_b);
            const bool neg_c = is_negative(raw_c);

            if constexpr (Fxd::frac_bits <= 0) {

                // offset >= w

                const auto [raw_d, ovf] = overflow::shl_real(raw_c, offset);
                if (ovf)
                    return unexpected{neg_c ? error::underflow : error::overflow};

                return safe::from_raw<Fxd>(high(raw_d));

            } else {

                // offset < w

                const auto bias = make_bias_for(Fxd::frac_bits, raw_c);
                const auto [biased_raw_c, ovf] = overflow::add(raw_c, bias);
                if (ovf)
                    return unexpected{error::overflow};

                const auto [raw_d, ovf2] = overflow::shl(biased_raw_c, offset);
                if (offset > 0 && ovf2)
                    return unexpected{neg_c ? error::underflow : error::overflow};

                return safe::from_raw<Fxd>(high(raw_d));
            }

        }


    } // namespace up


    inline
    namespace zero {

        // Multiply rounding to zero, when no fractional bits, with range check.
        template<fixed_point Fxd>
        requires (Fxd::frac_bits <= 0)
        [[nodiscard]]
        constexpr
        expected<Fxd, error>
        mul(Fxd a,
            Fxd b)
            noexcept
        {
            constexpr int w = type_width<typename Fxd::raw_type>;
            // offset used for shifting left.
            constexpr int offset = w - Fxd::frac_bits;
            static_assert(offset >= 0);

            const auto raw_a = a.raw_value;
            const auto raw_b = b.raw_value;
            const auto raw_c = raw::mul<Fxd::bits>(raw_a, raw_b);
            const bool neg_c = is_negative(raw_c);

            // no lower bits will be discarded, so no rounding needed
            const auto raw_d = shl_real(raw_c, offset);

            // unshift, check that the value matches
            if (offset > 0 && raw_c != shr_real(raw_d, offset))
                return unexpected{neg_c ? error::underflow : error::overflow};

            return safe::from_raw<Fxd>(high(raw_d));
        }



        // Multiply rounding to zero, when there are fractional bits, with range check.
        template<fixed_point Fxd>
        requires (Fxd::frac_bits > 0)
        [[nodiscard]]
        constexpr
        expected<Fxd, error>
        mul(Fxd a,
            Fxd b)
            noexcept
        {
            constexpr int w = detail::type_width<typename Fxd::raw_type>;
            // offset used for shifting left.
            constexpr int offset = w - Fxd::frac_bits;

            const auto raw_a = a.raw_value;
            const auto raw_b = b.raw_value;
            auto raw_c = raw::mul<Fxd::bits>(raw_a, raw_b);

            if (is_negative(raw_c)) {
                // negative numbers need a bias to zero when right-shifting
                const auto bias = make_bias_for(Fxd::frac_bits, raw_c);
                raw_c = detail::add(raw_c, bias);
            }

            const auto raw_d = detail::shl(raw_c, offset);
            // unshift, check that the value matches
            if (offset > 0 && raw_c != shr(raw_d, offset))
                return unexpected{is_negative(raw_c) ? error::underflow : error::overflow};

            return safe::from_raw<Fxd>(high(raw_d));
        }


    } // namespace zero


} // namespace fxd::detail::safe


#endif
