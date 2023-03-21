/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_COMPARE_HPP
#define LIBFXD_COMPARE_HPP

#include <compare>
#include <utility>

#include "detail/shift.hpp"


namespace fxd {


    /// Equality check for two similar `fxd::fixed`.
    template<fixed_point Fxd>
    [[nodiscard]]
    constexpr
    bool
    operator ==(Fxd a,
                Fxd b)
        noexcept
    {
        return a.raw_value == b.raw_value;
    }


    /// Equality check for dissimilar `fxd::fixed`.
    template<fixed_point A,
             fixed_point B>
    [[nodiscard]]
    constexpr
    bool
    operator ==(A a,
                B b)
        noexcept
    {
        bool different_signs = (a.raw_value < 0) != (b.raw_value < 0);
        if (different_signs)
            return false;

        auto raw_a = a.raw_value;
        auto raw_b = b.raw_value;

        /*
         * Algorithm: shift down the one with the most fractional bits,
         * to align the point.
         * If any 1 bit is lost, they're different.
         * Otherwise, just compare the lined-up values as regular integers.
         */
        constexpr int diff = A::frac_bits - B::frac_bits;
        if constexpr (diff > 0) {
            // shift down A
            auto [scaled_a, ovf] = detail::overflow::shr_real(raw_a, diff);
            if (ovf)
                return false;
            return std::cmp_equal(scaled_a, raw_b);
        } else {
            // shift down B
            auto [scaled_b, ovf] = detail::overflow::shr_real(raw_b, -diff);
            if (ovf)
                return false;
            return std::cmp_equal(raw_a, scaled_b);
        }
    }


    /// `fxd::fixed` == @e integer.
    template<fixed_point Fxd,
             std::integral I>
    [[nodiscard]]
    constexpr
    bool
    operator ==(Fxd a,
                I b)
        noexcept
    {
        if constexpr (Fxd::frac_bits < 0) {
            auto [hi_b, ovf] = detail::overflow::shr_real(b, -Fxd::frac_bits);
            if (ovf)
                return false;
            return std::cmp_equal(a.raw_value, hi_b);
        } else {
            auto [hi_a, ovf] = detail::overflow::shr_real(a.raw_value, Fxd::frac_bits);
            if (ovf)
                return false;
            return std::cmp_equal(hi_a, b);
        }
    }


    /// `fxd::fixed` == <em>floating-point</em>
    template<fixed_point Fxd,
             std::floating_point Flt>
    requires (requires (Fxd fxd) {to_float(fxd);})
    [[nodiscard]]
    constexpr
    bool
    operator ==(Fxd a,
                Flt b)
        noexcept
    {
        return to_float(a) == b;
    }



    template<fixed_point Fxd>
    [[nodiscard]]
    constexpr
    std::strong_ordering
    operator <=>(Fxd a,
                 Fxd b)
        noexcept
    {
        // workaround for GCC bug: it doesn't like mixing spaceship operator with bitfields
        using Raw = typename Fxd::raw_type;
        Raw ra = a.raw_value;
        Raw rb = b.raw_value;
        return ra <=> rb;
    }


    template<fixed_point A,
             fixed_point B>
    [[nodiscard]]
    constexpr
    std::strong_ordering
    operator <=>(A a,
                 B b)
        noexcept
    {
        bool a_neg = a.raw_value < 0;
        bool b_neg = b.raw_value < 0;
        if (a_neg && !b_neg)
            return std::strong_ordering::less;
        if (!a_neg && b_neg)
            return std::strong_ordering::greater;
        // after this, they have same sign

        /*
         * Algorithm: line up the point by shifting down the one with more fractional bits.
         *
         * If the high bits are enough to define the order, we don't need the
         * fractional bits.
         *
         * Otherwise, the bits shifted out decide if they're equal or different; if any
         * bit was shifted out, that value is larger. It works for both positive and
         * negative values.
         */

        constexpr int diff = A::frac_bits - B::frac_bits;
        const auto raw_a = a.raw_value;
        const auto raw_b = b.raw_value;

        if (raw_a == 0 && raw_b == 0)
            return std::strong_ordering::equal;

        if constexpr (diff > 0) {
            // shift down A
            auto [scaled_a, ovf] = detail::overflow::shr_real(raw_a, diff);
            if (std::cmp_not_equal(scaled_a, raw_b))
                return scaled_a <=> raw_b;
            if (ovf)
                return std::strong_ordering::greater;
            return std::strong_ordering::equal;
        } else {
            // shift down B
            auto [scaled_b, ovf] = detail::overflow::shr_real(raw_b, -diff);
            if (std::cmp_not_equal(raw_a, scaled_b))
                return raw_a <=> scaled_b;
            if (ovf)
                return std::strong_ordering::less;
            return std::strong_ordering::equal;
        }
    }


    template<fixed_point Fxd,
             std::integral I>
    [[nodiscard]]
    constexpr
    std::strong_ordering
    operator <=>(Fxd a,
                 I b)
        noexcept
    {
        if constexpr (Fxd::frac_bits < 0) {
            auto [hi_b, ovf] = detail::overflow::shr_real(b, -Fxd::frac_bits);
            auto cmp = a.raw_value <=> hi_b;
            if (cmp != 0 || !ovf) // top bits are different, or bottom bits are all zeros
                return cmp;
            // top bits are equal (a and b have same sign), and b has non-zero bottom bits
            return std::strong_ordering::less;
        } else {
            auto [int_a, ovf] = detail::overflow::shr_real(a.raw_value, Fxd::frac_bits);
            auto cmp = int_a <=> b;
            if (cmp != 0 || !ovf)
                return cmp;
            return std::strong_ordering::greater;
        }
    }


    template<fixed_point Fxd,
             std::floating_point Flt>
    requires (requires (Fxd a) {to_float(a);})
    [[nodiscard]]
    constexpr
    std::partial_ordering
    operator <=>(Fxd a,
                 Flt b)
        noexcept
    {
        return to_float(a) <=> b;
    }



}


#endif
