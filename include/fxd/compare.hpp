/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_COMPARE_HPP
#define LIBFXD_COMPARE_HPP


namespace fxd {


    template<fixed_point Fxd>
    constexpr
    bool
    operator ==(Fxd a,
                Fxd b)
        noexcept
    {
        return a.raw_value == b.raw_value;
    }


    template<fixed_point Fxd,
             std::integral I>
    constexpr
    bool
    operator ==(Fxd a,
                I b)
        noexcept
    {
        if constexpr (Fxd::frac_bits < 0) {
            auto [hi_b, ovf] = impl::overflow::shr_real(b, -Fxd::frac_bits);
            if (ovf)
                return false;
            return std::cmp_equal(a.raw_value, hi_b);
        } else {
            auto [hi_a, ovf] = impl::overflow::shr_real(a.raw_value, Fxd::frac_bits);
            if (ovf)
                return false;
            return std::cmp_equal(hi_a, b);
        }
    }


    template<std::integral I,
             fixed_point Fxd>
    constexpr
    bool
    operator ==(I a,
                Fxd b)
        noexcept
    {
        return b == a;
    }


    template<fixed_point Fxd,
             std::floating_point Flt>
    requires (requires (Fxd fxd) {to_float(fxd);})
    constexpr
    bool
    operator ==(Fxd a,
                Flt b)
        noexcept
    {
        return to_float(a) == b;
    }


    template<std::floating_point Flt,
             fixed_point Fxd>
    requires (requires (Fxd fxd) {to_float(fxd);})
    constexpr
    bool
    operator ==(Flt a,
                Fxd b)
        noexcept
    {
        return a == to_float(b);
    }



    template<fixed_point Fxd>
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


    template<fixed_point Fxd,
             std::integral I>
    constexpr
    std::strong_ordering
    operator <=>(Fxd a,
                 I b)
        noexcept
    {
        if constexpr (Fxd::frac_bits < 0) {
            auto [hi_b, ovf] = impl::overflow::shr_real(b, -Fxd::frac_bits);
            auto cmp = a.raw_value <=> hi_b;
            if (cmp != 0 || !ovf) // top bits are different, or bottom bits are all zeros
                return cmp;
            // top bits are equal (a and b have same sign), and b has non-zero bottom bits
            return std::strong_ordering::less;
        } else {
            auto [int_a, ovf] = impl::overflow::shr_real(a.raw_value, Fxd::frac_bits);
            auto cmp = int_a <=> b;
            if (cmp != 0 || !ovf)
                return cmp;
            return std::strong_ordering::greater;
        }
    }


    template<std::integral I,
             fixed_point Fxd>
    constexpr
    std::strong_ordering
    operator <=>(I a,
                 Fxd b)
        noexcept
    {
        if constexpr (Fxd::frac_bits < 0) {
            auto [hi_a, ovf] = impl::overflow::shr_real(a, -Fxd::frac_bits);
            auto cmp = hi_a <=> b.raw_value;
            if (cmp != 0 || !ovf) // top bits are different, or bottom bits are all zeros
                return cmp;
            // top bits are equal (a and b have the same sign), and a has non-zero bottom bits
            return std::strong_ordering::greater;
        } else {
            auto [int_b, ovf] = impl::overflow::shr_real(b.raw_value, Fxd::frac_bits);
            auto cmp = a <=> int_b;
            if (cmp != 0 || !ovf)
                return cmp;
            return std::strong_ordering::less;
        }
    }


    template<fixed_point Fxd,
             std::floating_point Flt>
    requires (requires (Fxd fxd) {to_float(fxd);})
    constexpr
    std::partial_ordering
    operator <=>(Fxd a,
                 Flt b)
        noexcept
    {
        return to_float(a) <=> b;
    }


    template<std::floating_point Flt,
             fixed_point Fxd>
    requires (requires (Fxd fxd) {to_float(fxd);})
    constexpr
    std::partial_ordering
    operator <=>(Flt a,
                 Fxd b)
        noexcept
    {
        return a <=> to_float(b);
    }


}


#endif
