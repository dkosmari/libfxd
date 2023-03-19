/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_DETAIL_SAFE_HPP
#define LIBFXD_DETAIL_SAFE_HPP

#include <cmath>
#include <compare>
#include <concepts>
#include <limits>

#include "../concepts.hpp"
#include "../error.hpp"
#include "../limits.hpp"

#include "add.hpp"
#include "bias.hpp"
#include "expected.hpp"
#include "shift.hpp"
#include "sub.hpp"
#include "tuple.hpp"


namespace fxd::detail::safe {

    // Constructors

    template<fixed_point Fxd,
             std::integral I>
    [[nodiscard]]
    constexpr
    expected<Fxd, error>
    from_raw(I val)
        noexcept
    {
        Fxd result = Fxd::from_raw(val);

        if constexpr (type_width<I> > Fxd::bits) {
            // check if significant bits got chopped off
            if (std::cmp_not_equal(result.raw_value, val))
                return unexpected{val < 0 ? error::underflow : error::overflow};
        }
        return result;
    }


    template<fixed_point Fxd>
    [[nodiscard]]
    constexpr
    expected<Fxd, error>
    make_fixed(Fxd src)
        noexcept
    {
        return src;
    }



    template<fixed_point Fxd,
             std::integral I>
    [[nodiscard]]
    constexpr
    expected<Fxd, error>
    make_fixed(I val)
        noexcept
    {

        if constexpr (Fxd::frac_bits < 0) {

            // Shifting right.
            if (val < 0)
                val += make_bias_for(-Fxd::frac_bits, val);

            auto raw = shr_real(val, -Fxd::frac_bits);
            return safe::from_raw<Fxd>(raw);

        } else {

            // Shifting left.
            // Use max bits available to minimize spurious overflows.
            using IWide = detail::max_int_for<I>;
            auto [raw, overflow] = overflow::shl_real<IWide>(val,
                                                             Fxd::frac_bits);

            if (overflow)
                return unexpected{val < 0 ? error::underflow : error::overflow};

            return safe::from_raw<Fxd>(raw);

        }

    }


    template<fixed_point Fxd,
             std::floating_point Flt>
    [[nodiscard]]
    expected<Fxd, error>
    make_fixed(Flt val)
        noexcept
    {
        using Lim = std::numeric_limits<Fxd>;

        if (!std::isfinite(val)) {
            if (std::isnan(val))
                return unexpected{error::not_a_number};
            return unexpected{val < 0 ? error::underflow : error::overflow};
        }

        if (val < static_cast<Flt>(Lim::lowest()))
            return unexpected{error::underflow};

        if (val > static_cast<Flt>(Lim::max()))
            return unexpected{error::overflow};

        return Fxd{val};
    }


    // Casting (fixed -> fixed)


    template<fixed_point Fxd>
    [[nodiscard]]
    constexpr
    expected<Fxd, error>
    fixed_cast(Fxd src)
        noexcept
    {
        return src;
    }


    template<fixed_point Dst,
             fixed_point Src>
    requires (!std::same_as<Src, Dst>)
    [[nodiscard]]
    constexpr
    expected<Dst, error>
    fixed_cast(Src src)
        noexcept
    {
        using SrcRaw = typename Src::raw_type;
        SrcRaw sraw = src.raw_value;

        if constexpr (!std::numeric_limits<Dst>::is_signed
                      &&
                      std::numeric_limits<Src>::is_signed)
            if (sraw < 0)
                return unexpected{error::underflow};

        constexpr int diff = Dst::frac_bits - Src::frac_bits;

        if constexpr (diff < 0) {

            // shifting right
            if (sraw < 0)
                sraw += make_bias_for(-diff, sraw);

            auto draw = shr_real(sraw, -diff);

            return safe::from_raw<Dst>(draw);

        } else {

            // shifting left
            // use more bits to avoid spurious overflows
            using SrcWide = max_int_for<SrcRaw>;

            auto [draw, overflow] = overflow::shl_real<SrcWide>(sraw, diff);
            if (overflow)
                return unexpected{sraw < 0 ? error::underflow : error::overflow};

            return safe::from_raw<Dst>(draw);

        }

    }



    // Conversions (fixed -> primitive)


    template<std::integral I,
             fixed_point Fxd>
    [[nodiscard]]
    constexpr
    expected<I, error>
    to_int(Fxd f)
        noexcept
    {
        using Raw = typename Fxd::raw_type;
        Raw raw = f.raw_value;

        using LimI = std::numeric_limits<I>;

        if constexpr (!LimI::is_signed && std::numeric_limits<Raw>::is_signed) {
            if (raw < 0)
                return unexpected{error::underflow};
        }

        if constexpr (Fxd::frac_bits >= 0) {

            if (raw < 0) // if negative, add a bias before shifting
                raw += make_bias_for(Fxd::frac_bits, raw);

            raw = shr_real(raw, Fxd::frac_bits);

            if (std::cmp_less(raw, LimI::min()))
                return unexpected{error::underflow};

            if (std::cmp_greater(raw, LimI::max()))
                return unexpected{error::overflow};

            return raw;

        } else {

            // Allow left-shifting to happen on a wider type.
            using RR = detail::max_int_for<Raw>;
            auto [result, ovf] = overflow::shl_real<RR>(raw, -Fxd::frac_bits);
            if (ovf)
                return unexpected{raw < 0 ? error::underflow : error::overflow};

            if (std::cmp_less(result, LimI::min()))
                return unexpected{error::underflow};

            if (std::cmp_greater(result, LimI::max()))
                return unexpected{error::overflow};

            return result;

        }

    }



    // Operators


    template<fixed_point Fxd>
    [[nodiscard]]
    constexpr
    expected<Fxd, error>
    incremented(Fxd f)
        noexcept
    {
        // if no integral bit, it always overflows
        if constexpr (Fxd::int_bits < 1)
            return unexpected{error::overflow};

        auto [result, carry] = overflow::add(f.raw_value, Fxd{1}.raw_value);
        if (carry)
            return unexpected{error::overflow};

        return safe::from_raw<Fxd>(result);
    }


    template<fixed_point Fxd>
    [[nodiscard]]
    constexpr
    expected<Fxd, error>
    decremented(Fxd f)
        noexcept
    {
        // if no integral bit, it always underflows
        if constexpr (Fxd::int_bits < 1)
            return unexpected{error::underflow};

        auto [result, overflow] = overflow::sub(f.raw_value, Fxd{1}.raw_value);
        if (overflow)
            return unexpected{error::underflow};

        return safe::from_raw<Fxd>(result);
    }


    template<fixed_point Fxd>
    [[nodiscard]]
    constexpr
    expected<Fxd, error>
    negate(Fxd f)
        noexcept
    {
        using Lim = std::numeric_limits<Fxd>;

        if constexpr (!Lim::is_signed) {
            // unsigned values can only be negated if they're zero
            if (f.raw_value == 0)
                return f;
            return unexpected{error::underflow};
        }

        if (f == Lim::lowest())
            return unexpected{error::overflow};

        return -f;
    }


    template<fixed_point Fxd>
    [[nodiscard]]
    constexpr
    expected<Fxd, error>
    add(Fxd a,
        Fxd b)
        noexcept
    {
        auto [result, overflow] = overflow::add(a.raw_value, b.raw_value);

        if (overflow) {
            if constexpr (std::numeric_limits<Fxd>::is_signed)
                return unexpected{a.raw_value < 0 ? error::underflow : error::overflow};
            else
                return unexpected{error::overflow};
        }

        return safe::from_raw<Fxd>(result);
    }


    template<fixed_point Fxd>
    [[nodiscard]]
    constexpr
    expected<Fxd, error>
    sub(Fxd a,
        Fxd b)
        noexcept
    {
        auto [result, overflow] = overflow::sub(a.raw_value, b.raw_value);

        if (overflow) {
            if constexpr (std::numeric_limits<Fxd>::is_signed)
                return unexpected{a.raw_value < 0 ? error::underflow : error::overflow};
            else
                return unexpected{error::underflow};
        }

        return safe::from_raw<Fxd>(result);
    }


    template<fixed_point Fxd>
    [[nodiscard]]
    constexpr
    expected<Fxd, error>
    abs(Fxd f)
        noexcept
    {
        if (f.raw_value >= 0)
            return f;
        return safe::negate(f);
    }


} // namespace fxd::detail::safe

#endif
