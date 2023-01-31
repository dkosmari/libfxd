/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_ROUND_MUL_HPP
#define LIBFXD_ROUND_MUL_HPP

#include "concepts.hpp"

#include "impl/add.hpp"
#include "impl/mul.hpp"
#include "impl/shift.hpp"


namespace fxd {


    namespace zero {

        template<fixed_point Fxd>
        constexpr
        Fxd
        mul(Fxd a,
            Fxd b)
            noexcept
        {
            constexpr int w = type_width<typename Fxd::raw_type>;

            // offset used for shifting left
            constexpr int offset = w - Fxd::frac_bits;

            const auto c = impl::mul<Fxd::bits>(a.raw_value, b.raw_value);

            if constexpr (Fxd::frac_bits <= 0) {

                // no lower bits will be lost, no rounding is needed
                const auto d = impl::shl(c, offset);
                return Fxd::from_raw(impl::last(d));

            } else {

                // will lose lower bits
                if (impl::is_negative(c)) {

                    // negative numbers need a bias to zero to round up
                    const auto bias = impl::make_bias_for(Fxd::frac_bits, c);
                    const auto biased_c = impl::add(c, bias);
                    const auto d = impl::shl(biased_c, offset);
                    return Fxd::from_raw(impl::last(d));

                } else {

                    const auto d = impl::shl(c, offset);
                    return Fxd::from_raw(impl::last(d));

                }

            }

        }

    } // namespace zero



    namespace up {

        template<fixed_point Fxd>
        constexpr
        Fxd
        mul(Fxd a,
            Fxd b)
            noexcept
        {
            constexpr int w = type_width<typename Fxd::raw_type>;

            // offset used for shifting left
            constexpr int offset = w - Fxd::frac_bits;

            const auto c = impl::mul<Fxd::bits>(a.raw_value, b.raw_value);

            if constexpr (Fxd::frac_bits <= 0) {

                const auto d = impl::shl(c, offset);
                return Fxd::from_raw(impl::last(d));

            } else {

                const auto bias = impl::make_bias_for(Fxd::frac_bits, c);
                const auto biased_c = impl::add(c, bias);
                const auto d = impl::shl(biased_c, offset);

                return Fxd::from_raw(impl::last(d));

            }

        }

    } // namespace up



    namespace down {

        // rounding down is the default behavior of >>, so no special handling is needed

        template<fixed_point Fxd>
        constexpr
        Fxd
        mul(Fxd a,
            Fxd b)
            noexcept
        {
            constexpr int w = type_width<typename Fxd::raw_type>;

            // offset used for shifting left
            constexpr int offset = w - Fxd::frac_bits;

            const auto c = impl::mul<Fxd::bits>(a.raw_value, b.raw_value);

            const auto d = impl::shl(c, offset);

            return Fxd::from_raw(impl::last(d));
        }

    } // namespace down


} // namespace fxd


#endif
