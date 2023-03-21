/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_OPERATORS_HPP
#define LIBFXD_OPERATORS_HPP

#include <compare>
#include <concepts>
#include <ostream>
#include <istream>

#include "concepts.hpp"
#include "limits.hpp"
#include "round-div.hpp"
#include "round-mul.hpp"
#include "traits.hpp"



/*
 * This file contains the unsafe, but fast, operators.
 *
 * They map directly to integer operators, with the same safety provisions as the
 * underlying integer type:
 *   - signed integers create UB on overflow; so do signed fixed points.
 *   - unsigned integers don't create UB; neither do unsigned fixed points.
 *
 * Different compilers offer options that control signed integer overflow, like '-ftrapv'
 * and '-fwrapv', thus making them defined behavior; these options directly apply to fixed
 * point overflows.
 */


namespace fxd {


    // ---------- //
    // Assignment //
    // ---------- //

    /// In-place addition.
    template<fixed_point Fxd,
             std::convertible_to<Fxd> T>
    constexpr
    Fxd&
    operator +=(Fxd& a,
                T b)
        noexcept
    {
        a.raw_value += Fxd{b}.raw_value;
        return a;
    }


    /// In-place subtraction.
    template<fixed_point Fxd,
             std::convertible_to<Fxd> T>
    constexpr
    Fxd&
    operator -=(Fxd& a,
                T b)
        noexcept
    {
        a.raw_value -= Fxd{b}.raw_value;
        return a;
    }


    /// In-place multiplication.
    template<fixed_point Fxd,
             std::convertible_to<Fxd> T>
    requires (!std::integral<T>)
    constexpr
    Fxd&
    operator *=(Fxd& a,
                T b)
        noexcept
    {
        return a = zero::mul<Fxd>(a, b);
    }


    /// In-place multiplication by integer.
    template<fixed_point Fxd,
             std::integral I>
    constexpr
    Fxd&
    operator *=(Fxd& a,
                I b)
        noexcept
    {
        a.raw_value *= b;
        return a;
    }


    /// In-place division.
    template<fixed_point Fxd,
             std::convertible_to<Fxd> T>
    requires (!std::integral<T>)
    constexpr
    Fxd&
    operator /=(Fxd& a,
                T b)
        noexcept
    {
        return a = zero::div<Fxd>(a, b);
    }


    /// In-place division by integer.
    template<fixed_point Fxd,
             std::integral I>
    constexpr
    Fxd&
    operator /=(Fxd& a,
                I b)
        noexcept
    {
        a.raw_value /= b;
        return a;
    }


    // ------------------- //
    // Increment/Decrement //
    // ------------------- //


    /// Pre-increment.
    template<fixed_point Fxd>
    constexpr
    Fxd&
    operator ++(Fxd& a)
        noexcept
    {
        a += 1;
        return a;
    }


    /// Pre-decrement.
    template<fixed_point Fxd>
    constexpr
    Fxd&
    operator --(Fxd& a)
        noexcept
    {
        a -= 1;
        return a;
    }


    /// Post-increment.
    template<fixed_point Fxd>
    constexpr
    Fxd
    operator ++(Fxd& a, int)
        noexcept
    {
        Fxd old = a;
        ++a;
        return old;
    }


    /// Post-decrement.
    template<fixed_point Fxd>
    constexpr
    Fxd
    operator --(Fxd& a, int)
        noexcept
    {
        Fxd old = a;
        --a;
        return old;
    }



    // ---------- //
    // Arithmetic //
    // ---------- //


    /// Unary `+`.
    template<fixed_point Fxd>
    constexpr
    Fxd
    operator +(Fxd a)
        noexcept
    {
        return a;
    }


    /// Negation.
    template<fixed_point Fxd>
    constexpr
    Fxd
    operator -(Fxd a)
        noexcept
    {
        return Fxd::from_raw(-a.raw_value);
    }


    /// Addition.
    template<typename A,
             typename B>
    requires (fixed_point<A> || fixed_point<B>)
    constexpr
    std::common_type_t<A, B>
    operator +(A a,
               B b)
        noexcept
    {
        using Fxd = std::common_type_t<A, B>;
        return Fxd::from_raw(Fxd{a}.raw_value
                             +
                             Fxd{b}.raw_value);
    }


    /// Subtraction.
    template<typename A,
             typename B>
    requires (fixed_point<A> || fixed_point<B>)
    constexpr
    std::common_type_t<A, B>
    operator -(A a,
               B b)
        noexcept
    {
        using Fxd = std::common_type_t<A, B>;
        return Fxd::from_raw(Fxd{a}.raw_value
                             -
                             Fxd{b}.raw_value);
    }


    /// Multiply: `fxd::fixed` * `fxd::fixed`
    template<fixed_point A,
             fixed_point B>
    constexpr
    std::common_type_t<A, B>
    operator *(A a,
               B b)
        noexcept
    {
        using Fxd = std::common_type_t<A, B>;
        return zero::mul<Fxd>(a, b);
    }


    /// Multiply: `fxd::fixed` * @e integer
    template<fixed_point Fxd,
             std::integral I>
    constexpr
    Fxd
    operator *(Fxd a,
               I b)
        noexcept
    {
        return Fxd::from_raw(a.raw_value * b);
    }


    /// Multiply: @e integer * `fxd::fixed`
    template<std::integral I,
             fixed_point Fxd>
    constexpr
    Fxd
    operator *(I a,
               Fxd b) noexcept
    {
        return Fxd::from_raw(a * b.raw_value);
    }


    /// Multiply: `fxd::fixed` * <em>floating-point</em>
    template<fixed_point Fxd,
             std::floating_point Flt>
    constexpr
    Fxd
    operator *(Fxd a,
               Flt b)
        noexcept
    {
        // TODO: optimize fixed * float
        return zero::mul<Fxd>(a, b);
    }


    /// Multiply: <em>floating-point</em> * `fxd::fixed`
    template<std::floating_point Flt,
             fixed_point Fxd>
    constexpr
    Fxd
    operator *(Flt a,
               Fxd b)
        noexcept
    {
        // TODO: optimize float * fixed
        return zero::mul<Fxd>(a, b);
    }



    /// Divide: `fxd::fixed` / `fxd::fixed`
    template<fixed_point A,
             fixed_point B>
    constexpr
    std::common_type_t<A, B>
    operator /(A a,
               B b)
        noexcept
    {
        using Fxd = std::common_type_t<A, B>;
        return zero::div<Fxd>(a, b);
    }


    /// Divide: `fxd::fixed` / @e integer
    template<fixed_point Fxd,
             std::integral I>
    constexpr
    Fxd
    operator /(Fxd a,
               I b)
        noexcept
    {
        return Fxd::from_raw(a.raw_value / b);
    }


    /// Divide: @e integer / `fxd::fixed`
    template<std::integral I,
             fixed_point Fxd>
    constexpr
    Fxd
    operator /(I a,
               Fxd b)
        noexcept
    {
        return Fxd{a} / b;
    }


    /// Divide: `fxd::fixed` / <em>floating-point</em>
    template<fixed_point Fxd,
             std::floating_point Flt>
    constexpr
    Fxd
    operator /(Fxd a,
               Flt b)
        noexcept
    {
        return zero::div<Fxd>(a, b);
    }


    /// Divide: <em>floating-point</em> / `fxd::fixed`.
    template<std::floating_point Flt,
             fixed_point Fxd>
    constexpr
    Fxd
    operator /(Flt a,
               Fxd b)
        noexcept
    {
        return zero::div<Fxd>(a, b);
    }


    // --- //
    // I/O //
    // --- //


    /// Output (by converting to a floating-point value).
    template<typename CharT,
             typename Traits,
             fixed_point Fxd>
    std::basic_ostream<CharT, Traits>&
    operator <<(std::basic_ostream<CharT, Traits>& out,
                Fxd f)
    {
        return out << to_float(f);
    }


    /// Input (by converting from a floatiung-point value).
    template<typename CharT,
             typename Traits,
             fixed_point Fxd>
    std::basic_istream<CharT, Traits>&
    operator >>(std::basic_istream<CharT, Traits>& in,
                Fxd& f)
    {
        typename std::numeric_limits<Fxd>::float_type ff;
        in >> ff;
        f = ff;
        return in;
    }



}


#endif
