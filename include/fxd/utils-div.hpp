#ifndef LIBFXD_UTILS_DIV_HPP
#define LIBFXD_UTILS_DIV_HPP

#include <algorithm>
#include <bit>
#include <concepts>
#include <type_traits>

#include "types.hpp"

#include "error.hpp"
#include "utils-expected.hpp"
#include "utils-overflow.hpp"
#include "utils-shift.hpp"


namespace fxd::utils::div {


    template<std::integral I>
    struct div_result {
        I quotient;
        int exponent;
        bool has_remainder;
    };


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
        }

        const I q = a / b;
        const I r = a % b;
        return div_result<I>{ q, 0, r != 0 };
    }


    template<int frac_bits,
             bool safe = false,
             std::integral I>
    requires (frac_bits > 0 && has_int_for<frac_bits + type_width<I>, I>)
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
        }

        using W = select_int_for<frac_bits + type_width<I>, I>;
        const W aa = shift::shl_real<W>(a, frac_bits);
        const W bb = b;
        const W q = aa / bb;
        const W r = aa % bb;
        return div_result<W>{ q, -frac_bits, r != 0 };
    }



    // performs extended division
    // returns the mantissa and exponent as a pair
    template<int frac_bits,
             bool safe = false,
             std::unsigned_integral U>
    requires (!has_int_for<frac_bits + type_width<U>, U>)
    expected<div_result<U>,
             error>
    div(U a,
        U b)
        noexcept
    {
        using shift::shl_real;

        const int expo_a = std::countl_zero(a);
        const int expo_b = std::countl_zero(b);
        // scale of the result
        const int expo_q = expo_b - expo_a;

        if (!b)
            return unexpected{!a ? error::not_a_number : error::overflow};

        if (!a)
            return div_result<U>{ 0, 0, false };

        a = a << expo_a;
        b = b << expo_b;

        U quo = a >= b;
        U rem = quo ? a - b : a;

        int i = 0;
        for (; rem && i < frac_bits + expo_q; ++i) {

            auto [new_rem, carry] = overflow::shl_real(rem, 1);
            rem = new_rem;

            if constexpr (safe) {
                auto [new_quo, ovf] = utils::overflow::shl_real(quo, 1);
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
    requires (!has_int_for<frac_bits + type_width<S>, S>)
    expected<div_result<S>,
             error>
    div(S a,
        S b)
        noexcept
    {
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


}


#endif
