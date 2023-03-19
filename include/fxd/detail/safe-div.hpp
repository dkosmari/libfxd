/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_DETAIL_SAFE_DIV_HPP
#define LIBFXD_DETAIL_SAFE_DIV_HPP

#include <limits>

#include "../concepts.hpp"

#include "bias.hpp"
#include "raw-div.hpp"
#include "expected.hpp"
#include "shift.hpp"
#include "types.hpp"

#include "../error.hpp"


namespace fxd::detail::safe {


    namespace down {

        template<fixed_point Fxd>
        [[nodiscard]]
        constexpr
        expected<Fxd, error>
        div(Fxd a,
            Fxd b)
            noexcept
        {
            const auto raw_a = a.raw_value;
            const auto raw_b = b.raw_value;
            const auto r = raw::div<Fxd::frac_bits, true>(raw_a, raw_b);
            if (!r)
                return unexpected{r.error()};

            auto [raw_c, expo, rem] = *r;

            const bool neg = (raw_a < 0) != (raw_b < 0);

            // offset used for shifting left
            const int offset = expo + Fxd::frac_bits;

            // When a/b is negative, it may have been rounded up.
            if (neg && rem) {
                if (raw_c == std::numeric_limits<decltype(raw_c)>::min())
                    return unexpected{error::underflow};
                --raw_c;
            }


            // Right-shifting always rounds down, so we don't need to bias it.

            const auto [raw_d, ovf] = overflow::shl(raw_c, offset);
            if (offset > 0 && ovf)
                return unexpected{neg ? error::underflow : error::overflow};

            return safe::from_raw<Fxd>(raw_d);
        }

    } // namespace down


    namespace up {

        template<fixed_point Fxd>
        [[nodiscard]]
        constexpr
        expected<Fxd, error>
        div(Fxd a,
            Fxd b)
            noexcept
        {
            const auto raw_a = a.raw_value;
            const auto raw_b = b.raw_value;
            const auto r = raw::div<Fxd::frac_bits, true>(raw_a, raw_b);

            if (!r)
                return unexpected{r.error()};

            auto [raw_c, expo, rem] = *r;

            // Note: detail::div() rounds to zero; so we might lose the info about which
            // side of zero the result was.
            const bool neg = (raw_a < 0) != (raw_b < 0);

            // offset used for shifting left
            const int offset = expo + Fxd::frac_bits;

            // When a/b is positive, it may have been rounded down.
            if (!neg && rem) {
                if (raw_c == std::numeric_limits<decltype(raw_c)>::max())
                    return unexpected{error::overflow};
                ++raw_c;
            }

            // Right-shifting will always round down.
            if (offset < 0) {
                const auto bias = make_bias_for(-offset, raw_c);
                const auto [biased_raw_c, ovf] = overflow::add(raw_c, bias);
                if (ovf)
                    return unexpected{error::overflow};
                raw_c = biased_raw_c;
            }

            const auto [raw_d, ovf] = overflow::shl(raw_c, offset);
            if (offset > 0 && ovf)
                return unexpected{neg ? error::underflow : error::overflow};

            return safe::from_raw<Fxd>(raw_d);
        }

    } // namespace up


    inline
    namespace zero {

        template<fixed_point Fxd>
        [[nodiscard]]
        constexpr
        expected<Fxd, error>
        div(Fxd a,
            Fxd b)
            noexcept
        {
            const auto raw_a = a.raw_value;
            const auto raw_b = b.raw_value;
            const auto r = raw::div<Fxd::frac_bits, true>(raw_a, raw_b);

            if (!r)
                return unexpected{r.error()};

            auto [raw_c, expo, rem] = *r;

            const int offset = expo + Fxd::frac_bits;

            if (raw_c < 0 && offset < 0)
                raw_c += make_bias_for(-offset, raw_c);

            const auto [raw_d, ovf] = overflow::shl(raw_c, offset);

            if (offset > 0 && ovf)
                return unexpected{raw_c < 0 ? error::underflow : error::overflow};

            return safe::from_raw<Fxd>(raw_d);
        }

    } // namespace zero


} // namespace fxd::detail::safe


#endif
