#ifndef LIBFXD_OPERATORS_HPP
#define LIBFXD_OPERATORS_HPP

#include <cmath>
#include <compare>
#include <concepts>
#include <csignal>
#include <ostream>
#include <istream>

#include "fixed.hpp"

#include "concepts.hpp"
#include "round-add.hpp"
#include "round-div.hpp"
#include "round-mul.hpp"
#include "round-sub.hpp"
#include "utils-div.hpp"
#include "utils-mul.hpp"
#include "utils-shift.hpp"
#include "utils.hpp"


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


    template<int Int,
             int Frac,
             typename Raw>
    template<std::integral I>
    ALWAYS_INLINE
    constexpr
    fixed<Int, Frac, Raw>&
    fixed<Int, Frac, Raw>::operator =(I i)
        noexcept
    {
        raw_value = utils::shift::shl<Raw>(i, frac_bits);
        return *this;
    }


    template<int Int,
             int Frac,
             typename Raw>
    template<std::floating_point F>
    constexpr
    fixed<Int, Frac, Raw>&
    fixed<Int, Frac, Raw>::operator =(F f)
        noexcept
    {
        using std::ldexp;
        raw_value = static_cast<raw_type>(ldexp(f, frac_bits));
        return *this;
    }


    template<fixed_point Fxd,
             std::convertible_to<Fxd> T>
    ALWAYS_INLINE
    constexpr
    Fxd&
    operator +=(Fxd& a,
                const T& b)
        noexcept
    {
        return a = round::add<Fxd>(a, b);
    }


    template<fixed_point Fxd,
             std::convertible_to<Fxd> T>
    ALWAYS_INLINE
    constexpr
    Fxd&
    operator -=(Fxd& a,
                const T& b)
        noexcept
    {
        return a = round::sub<Fxd>(a, b);
    }


    template<fixed_point Fxd,
             std::convertible_to<Fxd> T>
    ALWAYS_INLINE
    constexpr
    Fxd&
    operator *=(Fxd& a,
                const T& b)
        noexcept
    {
        return a = round::zero::mul<Fxd>(a, b);
    }


    template<fixed_point Fxd,
             std::convertible_to<Fxd> T>
    ALWAYS_INLINE
    constexpr
    Fxd&
    operator /=(Fxd& a,
                const T& b)
        noexcept
    {
        return a = round::zero::div<Fxd>(a, b);
    }



    // ------------------- //
    // Increment/Decrement //
    // ------------------- //


    template<fixed_point Fxd>
    ALWAYS_INLINE
    constexpr
    Fxd&
    operator ++(Fxd& a)
        noexcept
    {
        a += 1;
        return a;
    }


    template<fixed_point Fxd>
    ALWAYS_INLINE
    constexpr
    Fxd&
    operator --(Fxd& a)
        noexcept
    {
        a-= 1;
        return a;
    }


    template<fixed_point Fxd>
    ALWAYS_INLINE
    constexpr
    Fxd
    operator ++(Fxd& a, int)
        noexcept
    {
        Fxd old = a;
        a += 1;
        return old;
    }


    template<fixed_point Fxd>
    ALWAYS_INLINE
    constexpr
    Fxd
    operator --(Fxd& a, int)
        noexcept
    {
        Fxd old = a;
        a -= 1;
        return old;
    }



    // ---------- //
    // Arithmetic //
    // ---------- //


    template<fixed_point Fxd>
    ALWAYS_INLINE
    constexpr
    const Fxd&
    operator +(const Fxd& a)
        noexcept
    {
        return a;
    }


    template<fixed_point Fxd>
    ALWAYS_INLINE
    constexpr
    Fxd
    operator -(const Fxd& a)
        noexcept
    {
        return Fxd::from_raw(-a.raw_value);
    }



    template<typename A,
             typename B>
    requires(fixed_point<A> || fixed_point<B>)
    ALWAYS_INLINE
    constexpr
    std::common_type_t<A, B>
    operator +(A a, B b)
        noexcept
    {
        using Fxd = std::common_type_t<A, B>;
        return round::add<Fxd>(a, b);
    }


    template<typename A,
             typename B>
    requires(fixed_point<A> || fixed_point<B>)
    ALWAYS_INLINE
    constexpr
    std::common_type_t<A, B>
    operator -(A a, B b)
        noexcept
    {
        using Fxd = std::common_type_t<A, B>;
        return round::sub<Fxd>(a, b);
    }


    template<typename A,
             typename B>
    requires(fixed_point<A> || fixed_point<B>)
    ALWAYS_INLINE
    constexpr
    std::common_type_t<A, B>
    operator *(A a, B b)
        noexcept
    {
        using Fxd = std::common_type_t<A, B>;
        return round::zero::mul<Fxd>(a, b);
    }


    template<typename A,
             typename B>
    requires(fixed_point<A> || fixed_point<B>)
    ALWAYS_INLINE
    constexpr
    std::common_type_t<A, B>
    operator /(A a, B b)
        noexcept
    {
        using Fxd = std::common_type_t<A, B>;
        return round::zero::div<Fxd>(a, b);
    }



    // --- //
    // I/O //
    // --- //


    template<typename CharT,
             typename Traits,
             fixed_point Fxd>
    std::basic_ostream<CharT, Traits>&
    operator <<(std::basic_ostream<CharT, Traits>& out,
                const Fxd& f)
    {
        return out << to_float(f);
    }


    template<typename CharT,
             typename Traits,
             fixed_point Fxd>
    std::basic_istream<CharT, Traits>&
    operator >>(std::basic_istream<CharT, Traits>& in,
                Fxd& f)
    {
        typename Fxd::float_type ff;
        if (in >> ff)
            f = ff;
        return in;
    }




}


#endif
