/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_ROUND_MUL_HPP
#define LIBFXD_ROUND_MUL_HPP

#include "concepts.hpp"

#include "detail/add.hpp"
#include "detail/raw-mul.hpp"
#include "detail/shift.hpp"


namespace fxd {

    namespace zero {

        /// Multiply rounding to zero.
        template<fixed_point Fxd>
        constexpr
        Fxd
        mul(Fxd a,
            Fxd b)
            noexcept
        {
            constexpr int w = detail::type_width<typename Fxd::raw_type>;

            // offset used for shifting left
            constexpr int offset = w - Fxd::frac_bits;

            const auto raw_a = a.raw_value;
            const auto raw_b = b.raw_value;
            const auto c = detail::raw::mul<Fxd::bits>(raw_a, raw_b);

            if constexpr (Fxd::frac_bits <= 0) {

                // no lower bits will be lost, no rounding is needed
                const auto d = detail::shl(c, offset);
                return Fxd::from_raw(detail::high(d));

            } else {

                // will lose lower bits
                if (detail::is_negative(c)) {

                    // negative numbers need a bias to zero to round up
                    const auto bias = detail::make_bias_for(Fxd::frac_bits, c);
                    const auto biased_c = detail::add(c, bias);
                    const auto d = detail::shl(biased_c, offset);
                    return Fxd::from_raw(detail::high(d));

                } else {

                    const auto d = detail::shl(c, offset);
                    return Fxd::from_raw(detail::high(d));

                }

            }

        }

    } // namespace zero



    namespace up {

        /// Multiply rounding up.
        template<fixed_point Fxd>
        constexpr
        Fxd
        mul(Fxd a,
            Fxd b)
            noexcept
        {
            constexpr int w = detail::type_width<typename Fxd::raw_type>;

            // offset used for shifting left
            constexpr int offset = w - Fxd::frac_bits;

            const auto raw_a = a.raw_value;
            const auto raw_b = b.raw_value;
            const auto c = detail::raw::mul<Fxd::bits>(raw_a, raw_b);

            if constexpr (Fxd::frac_bits <= 0) {

                const auto d = detail::shl(c, offset);
                return Fxd::from_raw(detail::high(d));

            } else {

                const auto bias = detail::make_bias_for(Fxd::frac_bits, c);
                const auto biased_c = detail::add(c, bias);
                const auto d = detail::shl(biased_c, offset);

                return Fxd::from_raw(detail::high(d));

            }

        }

    } // namespace up



    namespace down {

        /// Multiply rounding down.
        template<fixed_point Fxd>
        constexpr
        Fxd
        mul(Fxd a,
            Fxd b)
            noexcept
        {
            constexpr int w = detail::type_width<typename Fxd::raw_type>;

            // offset used for shifting left
            constexpr int offset = w - Fxd::frac_bits;

            const auto raw_a = a.raw_value;
            const auto raw_b = b.raw_value;
            const auto c = detail::raw::mul<Fxd::bits>(raw_a, raw_b);

            const auto d = detail::shl(c, offset);

            return Fxd::from_raw(detail::high(d));
        }

    } // namespace down


} // namespace fxd


#endif
