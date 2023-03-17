/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_MATH_HPP
#define LIBFXD_MATH_HPP

#include <algorithm>
#include <bit>
#include <cassert>
#include <cerrno>
#include <numeric>
#include <utility>

#include "casting.hpp"
#include "compare.hpp"
#include "concepts.hpp"
#include "constructors.hpp"
#include "limits.hpp"
#include "operators.hpp"
#include "round-div.hpp"
#include "round-mul.hpp"

#include "impl/add.hpp"
#include "impl/shift.hpp"


namespace fxd {


    /// Same as `std::abs()`.
    template<fixed_point Fxd>
    constexpr
    Fxd
    abs(Fxd f)
        noexcept
    {
        return f.raw_value < 0 ? -f : +f;
    }


    /// Same as `std::fdim()`.
    template<fixed_point Fxd>
    constexpr
    Fxd
    fdim(Fxd a,
         Fxd b)
        noexcept
    {
        if (a > b)
            return a - b;
        else
            return 0;
    }


    /// Same as `std::fma()`.
    template<fixed_point Fxd>
    constexpr
    Fxd
    fma(Fxd a,
        Fxd b,
        Fxd c)
        noexcept
    {
        return a * b + c;
    }


    /// Same as `std::ilogb()`.
    template<fixed_point Fxd>
    constexpr
    int
    ilogb(Fxd x)
        noexcept
    {
        if constexpr (std::numeric_limits<Fxd>::is_signed) {
            if (x < 0) {
                if (x == std::numeric_limits<Fxd>::lowest())
                    return Fxd::int_bits - 1;
                x = -x;
            }
        }
        if (!x)
            return std::numeric_limits<int>::min();

        using URaw = std::make_unsigned_t<typename Fxd::raw_type>;
        URaw rx = x.raw_value;
        return std::bit_width(rx) - 1 - Fxd::frac_bits;
    }


    namespace zero {
        /// Same as `std::ldexp()`.
        template<fixed_point Fxd>
        constexpr
        Fxd
        ldexp(Fxd x,
              int exp)
            noexcept
        {
            if (exp >= 0)
                return Fxd::from_raw(x.raw_value << exp);

            auto [y, ovf] = impl::overflow::shr_real(x.raw_value, -exp);
            if (y < 0 && ovf)
                ++y;
            return Fxd::from_raw(y);
        }
    }

    using zero::ldexp;


    namespace down {
        /// @copydoc zero::ldexp()
        template<fixed_point Fxd>
        constexpr
        Fxd
        ldexp(Fxd x,
              int exp)
            noexcept
        {
            return Fxd::from_raw(impl::shl(x.raw_value, exp));
        }
    }


    namespace up {
        /// @copydoc zero::ldexp()
        template<fixed_point Fxd>
        constexpr
        Fxd
        ldexp(Fxd x,
              int exp)
            noexcept
        {
            if (exp >= 0)
                return Fxd::from_raw(x.raw_value << exp);

            auto [y, ovf] = impl::overflow::shr_real(x.raw_value, -exp);
            if (ovf)
                ++y;
            return Fxd::from_raw(y);
        }
    }




    /// Same as `std::midpoint()`.
    template<fixed_point Fxd>
    constexpr
    Fxd
    midpoint(Fxd a,
             Fxd b)
        noexcept
    {
        auto mid = std::midpoint(a.raw_value, b.raw_value);
        return Fxd::from_raw(mid);
    }


    /// Same as `std::nextafter()`.
    template<fixed_point Fxd>
    constexpr
    Fxd
    nextafter(Fxd from,
              Fxd to)
        noexcept
    {
        constexpr Fxd e = std::numeric_limits<Fxd>::epsilon();
        if (from < to)
            return from + e;
        if (from > to)
            return from - e;
        return to;
    }



    /**
     * @brief Same as `std::sqrt()`.
     *
     * @note If the argument `x` is negative, `errno` is set to `EDOM`.
     */
    template<unsigned_fixed_point Fxd>
    requires (Fxd::int_bits > 0 && Fxd::frac_bits >= 0)
    constexpr
    Fxd
    sqrt(Fxd x)
        noexcept
    {
        if (!x)
            return x;

        // Babylonian/Heron/Newton-Raphson method

        int i = 0;
        Fxd old_b;

        // invariant: a <= b
        Fxd a, b = std::max(Fxd{1}, x);
#if 1
        // Try to find a better initial guess.
        int ib = ilogb(x);
        b = down::ldexp(x, - ib / 2);
#endif

        do {

            if (++i > Fxd::bits)
                break;

            assert(b > 0u);
            a = down::div(x, b);
            assert(a >= 0u);

            old_b = b;
            b = midpoint(b, a);

        } while (old_b != b);

        for (;;) {
            Fxd a_next = a + std::numeric_limits<Fxd>::epsilon();
            if (up::mul(a_next, a_next) <= x)
                a = a_next;
            else
                break;
        }

        return a;
    }


    /// @copybrief `fxd::sqrt()`
    template<signed_fixed_point Fxd>
    requires (Fxd::int_bits > 1 && Fxd::frac_bits >= 0)
    constexpr
    Fxd
    sqrt(Fxd x)
        noexcept
    {
        if (x < 0) {
            errno = EDOM;
            return 0;
        }
        auto ux = fxd::fixed_cast<fxd::make_unsigned_t<Fxd>>(x);
        auto r = sqrt(ux);
        return fixed_cast<Fxd>(r);
    }



    template<unsigned_fixed_point Fxd>
    requires (Fxd::int_bits > 0 && Fxd::frac_bits >= 0)
    Fxd
    sqrt_bin(Fxd x)
        noexcept
    {
        if (!x)
            return x;

        using Lim = std::numeric_limits<Fxd>;

        const Fxd one = 1;

        // maximum root bit that can be tested without overflow
        const int top_bit = (Lim::max_bit <= 17
                             ? Lim::max_bit - 1
                             : ilogb(x)) / 2;
        // minimum root bit that can be tested without rounding
        const int bot_bit = Lim::min_bit / 2;

        Fxd r = 0;
        Fxd e = x;

        // compute integral part
        for (int b = top_bit; b >= 0; --b) {
            Fxd d = down::ldexp(r, b + 1) + down::ldexp(one, 2 * b);
            if (d <= e) {
                e -= d;
                r += down::ldexp(one, b);
                if (!e)
                    return r;
            }
        }

        // compute fractional part
        for (int b = -1; b >= bot_bit; --b) {
            Fxd d = down::ldexp(r, b + 1) + down::ldexp(one, 2 * b);
            if (d <= e) {
                e -= d;
                r += down::ldexp(one, b);
                if (!e)
                    break;
            }
        }

        if (Lim::min_bit >= -32) {

            // Try setting each unset bit to 1, and check the square.
            for (int b = bot_bit - 1; b >= Lim::min_bit; --b) {
                Fxd next_r = r + down::ldexp(one, b);
                if (up::mul(next_r, next_r) <= x)
                    r = next_r;
            }

        } else {

            // Continue with Newton-Raphson method

            int i = 0;

            Fxd a;
            Fxd b = r + down::ldexp(one, bot_bit);
            Fxd old_b;

            do {

                if (++i > Fxd::frac_bits)
                    break;

                a = down::div(x, b);

                old_b = b;
                b = midpoint(b, a);

            } while (old_b > b);

            for (;;) {
                Fxd a_next = a + std::numeric_limits<Fxd>::epsilon();
                if (up::mul(a_next, a_next) <= x)
                    a = a_next;
                else
                    break;
            }

            return a;
        }

        return r;
    }


    template<signed_fixed_point Fxd>
    requires (Fxd::int_bits > 1 && Fxd::frac_bits >= 0)
    Fxd
    sqrt_bin(Fxd x)
        noexcept
    {
        if (x < 0) {
            errno = EDOM;
            return 0;
        }
        auto ux = fxd::fixed_cast<fxd::make_unsigned_t<Fxd>>(x);
        auto r = sqrt_bin(ux);
        return fixed_cast<Fxd>(r);
    }


} // namespace fxd


#endif
