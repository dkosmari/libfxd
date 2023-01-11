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
#include "utils-div.hpp"
#include "utils-mul.hpp"
#include "utils-shift.hpp"


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
             typename T>
    template<std::integral I>
    constexpr
    fixed<Int, Frac, T>&
    fixed<Int, Frac, T>::operator =(I i)
        noexcept
    {
        raw_value = utils::shift::shl<T>(i, frac_bits);
        return *this;
    }


    template<int Int,
             int Frac,
             typename T>
    template<std::floating_point F>
    constexpr
    fixed<Int, Frac, T>&
    fixed<Int, Frac, T>::operator =(F f)
        noexcept
    {
        using std::ldexp;
        raw_value = static_cast<raw_type>(ldexp(f, frac_bits));
        return *this;
    }


    template<fixed_point Fxd,
             std::convertible_to<Fxd> T>
    constexpr
    Fxd&
    operator +=(Fxd& a,
                const T& b)
        noexcept
    {
        a.raw_value += Fxd{b}.raw_value;
        return a;
    }


    template<fixed_point Fxd,
             std::convertible_to<Fxd> T>
    constexpr
    Fxd&
    operator -=(Fxd& a,
                const T& b)
        noexcept
    {
        a.raw_value -= Fxd{b}.raw_value;
        return a;
    }


    template<fixed_point Fxd,
             std::convertible_to<Fxd> T>
    constexpr
    Fxd&
    operator *=(Fxd& a,
                const T& b_)
        noexcept
    {
        const Fxd b{b_};
        using R = typename Fxd::raw_type;

        if constexpr (has_wider_v<R>) {
            using W = wider_t<R>;
            const W aa = a.raw_value;
            const W bb = b.raw_value;
            const W cc = aa * bb;
            const W dd = utils::shift::shrz<W>(cc, Fxd::frac_bits);
            a.raw_value = dd;
        } else {
            const auto c = utils::mul::mul(a.raw_value,
                                           b.raw_value);
            const auto d = utils::shift::shrz(c, Fxd::frac_bits);
            a.raw_value = utils::tuple::first(d);
        }

        return a;
    }


    template<fixed_point Fxd,
             std::convertible_to<Fxd> T>
    constexpr
    Fxd&
    operator /=(Fxd& a,
                const T& b_)
        noexcept
    {
        const Fxd b{b_};
        using R = typename Fxd::raw_type;

        if constexpr (has_wider_v<R>) {

            using W = wider_t<R>;
            const W aa = utils::shift::shlz<W>(a.raw_value, Fxd::frac_bits);
            const W bb = b.raw_value;
            const W cc = aa / bb;
            a.raw_value = cc;

        } else {

            const bool neg_a = a.raw_value < 0;
            const bool neg_b = b.raw_value < 0;

            using U = std::make_unsigned_t<R>;
            U ua = a.raw_value;
            U ub = b.raw_value;

            if (neg_a)
                ua = -ua;

            if (neg_b)
                ub = -ub;

            auto q = utils::div::div<Fxd::frac_bits>(ua, ub);

            constexpr int w = type_width<U>;
            auto [lo, mi, hi] = utils::shift::shrz(q, 2 * w - Fxd::frac_bits);
            a.raw_value = neg_a == neg_b ? lo : -lo;

        }

        return a;
    }



    // ------------------- //
    // Increment/Decrement //
    // ------------------- //


    template<fixed_point Fxd>
    constexpr
    Fxd&
    operator ++(Fxd& a)
        noexcept
    {
        a.raw_value += Fxd::one;
        return a;
    }


    template<fixed_point Fxd>
    constexpr
    Fxd&
    operator --(Fxd& a)
        noexcept
    {
        a.raw_value -= Fxd::one;
        return a;
    }


    template<fixed_point Fxd>
    constexpr
    Fxd
    operator ++(Fxd& a, int)
        noexcept
    {
        Fxd old = a;
        a.raw_value += Fxd::one;
        return old;
    }


    template<fixed_point Fxd>
    constexpr
    Fxd
    operator --(Fxd& a, int)
        noexcept
    {
        Fxd old = a;
        a.raw_value -= Fxd::one;
        return old;
    }



    // ---------- //
    // Arithmetic //
    // ---------- //


    template<fixed_point Fxd>
    constexpr
    const Fxd&
    operator +(const Fxd& a)
        noexcept
    {
        return a;
    }


    template<fixed_point Fxd>
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
    constexpr
    std::common_type_t<A, B>
    operator +(A a, B b)
        noexcept
    {
        using Fxd = std::common_type_t<A, B>;
        Fxd fa{a};
        return fa += b;
    }


    template<typename A,
             typename B>
    requires(fixed_point<A> || fixed_point<B>)
    constexpr
    std::common_type_t<A, B>
    operator -(A a, B b)
        noexcept
    {
        using Fxd = std::common_type_t<A, B>;
        Fxd fa{a};
        return fa -= b;
    }


    template<typename A,
             typename B>
    requires(fixed_point<A> || fixed_point<B>)
    constexpr
    std::common_type_t<A, B>
    operator *(A a, B b)
        noexcept
    {
        using Fxd = std::common_type_t<A, B>;
        Fxd fa{a};
        return fa *= b;
    }


    template<typename A,
             typename B>
    requires(fixed_point<A> || fixed_point<B>)
    constexpr
    std::common_type_t<A, B>
    operator /(A a, B b)
        noexcept
    {
        using Fxd = std::common_type_t<A, B>;
        Fxd fa{a};
        return fa /= b;
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
                const Fxd& f)
    {
        typename Fxd::float_type ff;
        if (in >> ff)
            f = ff;
        return in;
    }




}


#endif
