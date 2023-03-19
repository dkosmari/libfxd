/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_DETAIL_RAW_DIV_HPP
#define LIBFXD_DETAIL_RAW_DIV_HPP

#include <algorithm>
#include <bit>
#include <concepts>
#include <type_traits>

#include "expected.hpp"
#include "shift.hpp"
#include "types.hpp"

#include "../error.hpp"


namespace fxd::detail::raw {


    template<std::integral I>
    struct div_result {
        I quotient;
        int exponent;
        bool has_remainder;
    };


    // When only integer division is required.
    template<int frac_bits,
             bool safe = false,
             std::integral I>
    requires (frac_bits <= 0)
    expected<div_result<I>,
             error>
    div(I a,
        I b)
        noexcept
    {
        if constexpr (safe) {
            if (!b) {
                if (!a)
                    return unexpected{error::not_a_number};
                return unexpected{a < 0 ? error::underflow : error::overflow};
            }

            // protect against INT_MIN / -1
            if constexpr (std::numeric_limits<I>::is_signed) {
                if (a == std::numeric_limits<I>::min() && b == -1)
                    return unexpected{error::overflow};
            }
        }

        const I q = a / b;
        const I r = a % b;
        return div_result<I>{ q, 0, r != 0 };
    }



    template<int frac_bits,
             typename I>
    concept can_use_int_div = has_int_for<frac_bits + type_width<I>, I>;


    // When fractional bits are required, but they fit in a larger integer.
    template<int frac_bits,
             bool safe = false,
             std::integral I>
    requires (frac_bits > 0 && can_use_int_div<frac_bits, I>)
    expected<div_result<select_int_for<frac_bits + type_width<I>, I>>,
             error>
    div(I a,
        I b)
        noexcept
    {
        if constexpr (safe) {
            if (!b) {
                if (!a)
                    return unexpected{error::not_a_number};
                return unexpected{a < 0 ? error::underflow : error::overflow};
            }

            // protect against INT_MIN / -1
            if constexpr (std::numeric_limits<I>::is_signed) {
                if (a == std::numeric_limits<I>::min() && b == -1)
                    return unexpected{error::overflow};
            }
        }

        using W = select_int_for<frac_bits + type_width<I>, I>;
        const W aa = static_cast<W>(a) << frac_bits;
        const W bb = b;
        const W q = aa / bb;
        const W r = aa % bb;

        if constexpr (safe) {
            if (q > std::numeric_limits<I>::max())
                return unexpected{error::overflow};
            if (q < std::numeric_limits<I>::min())
                return unexpected{error::underflow};
        }

        return div_result<W>{ q, -frac_bits, r != 0 };
    }






    // Fallback implementation: long binary division.

    template<int frac_bits,
             bool safe = false,
             std::unsigned_integral U>
    requires (!can_use_int_div<frac_bits, U>)
    expected<div_result<U>,
             error>
    div(U a,
        U b)
        noexcept
    {

#ifdef LIBFXD_PARTIAL_INT128

        if constexpr (type_width<U> + frac_bits <= 128) {

            if constexpr (safe) {
                if (!b)
                    return unexpected{!a ? error::not_a_number : error::overflow};
                if (!a)
                    return div_result<U>{ 0, 0, false };
            }

            using UU = uint128_t;
            const UU aa = static_cast<UU>(a) << frac_bits;
            const UU bb = b;
            UU q = aa / bb;
            const UU r = aa % bb;
            int expo = -frac_bits;

            if constexpr (safe)
                if (q > std::numeric_limits<U>::max())
                    return unexpected{error::overflow};

            return div_result<U>{
                static_cast<U>(q),
                expo,
                r != 0
            };
        }

#endif // LIBFXD_PARTIAL_INT128

        if (!b)
            return unexpected{!a ? error::not_a_number : error::overflow};
        if (!a)
            return div_result<U>{ 0, 0, false };

        const int expo_a = std::countl_zero(a);
        const int expo_b = std::countl_zero(b);
        // scale of the result
        const int expo_q = expo_b - expo_a;


        a = a << expo_a;
        b = b << expo_b;

        U quo = a >= b;
        U rem = quo ? a - b : a;

        int i = 0;
        for (; rem && i < frac_bits + expo_q; ++i) {

            auto [new_rem, carry] = overflow::shl_real(rem, 1);
            rem = new_rem;

            if constexpr (safe) {
                auto [new_quo, ovf] = overflow::shl_real(quo, 1);
                if (ovf)
                    return unexpected{error::overflow};
                quo = new_quo;
            } else {
                quo <<= 1;
            }

            if (carry || rem >= b) {
                quo |= 1;
                rem -= b;
            }
        }

        return div_result<U>{ quo, expo_q - i, rem != 0 };
    }



    template<int frac_bits,
             bool safe = false,
             std::signed_integral S>
    requires (!can_use_int_div<frac_bits, S>)
    expected<div_result<S>,
             error>
    div(S a,
        S b)
        noexcept
    {

#ifdef LIBFXD_PARTIAL_INT128

        if constexpr (type_width<S> + frac_bits <= 128) {

            if constexpr (safe) {
                if (!b)
                    return unexpected{!a
                        ? error::not_a_number
                        : (a < 0
                           ? error::underflow
                           : error::overflow)};
                if (!a)
                    return div_result<S>{ 0, 0, false };

                // protect against INT_MIN / -1
                if (a == std::numeric_limits<S>::min() && b == -1)
                    return unexpected{error::overflow};
            }

            using SS = int128_t;
            const SS aa = static_cast<SS>(a) << frac_bits;
            const SS bb = b;
            SS q = aa / bb;
            const SS r = aa % bb;
            int expo = -frac_bits;

            if constexpr (safe) {
                if (q > std::numeric_limits<S>::max())
                    return unexpected{error::overflow};
                if (q < std::numeric_limits<S>::min())
                    return unexpected{error::underflow};
            }

            return div_result<S>{
                static_cast<S>(q),
                expo,
                r != 0
            };
        }
#endif // LIBFXD_PARTIAL_INT128

        if (!b)
            return unexpected{!a
                ? error::not_a_number
                : (a < 0
                   ? error::underflow
                   : error::overflow)};
        if (!a)
            return div_result<S>{ 0, 0, false };

        const bool neg_a = a < 0;
        const bool neg_b = b < 0;
        const bool neg_c = neg_a != neg_b;

        using U = std::make_unsigned_t<S>;

        U ua = a;
        U ub = b;

        if (neg_a)
            ua = -ua;
        if (neg_b)
            ub = -ub;

        auto r = div<frac_bits, safe>(ua, ub);
        if (!r) {
            const error e = r.error();
            if (e == error::overflow && neg_c)
                return unexpected{error::underflow};
            return unexpected{e};
        }

        auto [uc, scale, rem] = *r;

        if (static_cast<S>(uc) < 0) {
            auto [new_uc, new_rem] = overflow::shr_real(uc, 1);
            uc = new_uc;
            rem |= new_rem;
            ++scale;
        }

        if (neg_c)
            uc = -uc;

        const S c = uc;
        return div_result<S>{ c, scale, rem };
    }


} // namespace fxd::detail::raw


#endif
