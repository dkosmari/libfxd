#ifndef LIBFXD_OPERATORS_HPP
#define LIBFXD_OPERATORS_HPP

#include <cmath>
#include <compare>
#include <concepts>
#include <csignal>
#include <ostream>

#include "fixed.hpp"

#include "concepts.hpp"
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
             typename T>
    template<std::integral I>
    constexpr
    fixed<Int, Frac, T>&
    fixed<Int, Frac, T>::operator =(I i)
        noexcept
    {
        raw_value = utils::shl<T>(i, frac_bits);
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
            a.raw_value = utils::shrz<W>(cc, Fxd::frac_bits);
        } else {
            const auto c = utils::full_mult(a.raw_value,
                                            b.raw_value);
            a.raw_value = utils::shrz(c, Fxd::frac_bits).first;
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
            const W aa = utils::shlz<W>(a.raw_value, Fxd::frac_bits);
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

            auto q = utils::full_div<Fxd::frac_bits>(ua, ub);

            // how many extra bits the division will calculate
            constexpr int extra = std::max(0, Fxd::frac_bits - utils::type_width<U>);
            U r = utils::shrz(q, utils::type_width<U> - Fxd::frac_bits + extra).first;
            a.raw_value = neg_a == neg_b ? r : -r;
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


    template<fixed_point Fxd>
    std::ostream&
    operator <<(std::ostream& out,
                const Fxd& f)
    {
        return out << to_float(f);
    }






}


#endif
