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

            auto q = utils::full_div(ua, ub, Fxd::frac_bits);

            U r = utils::shrz(q, utils::type_width<U> - Fxd::frac_bits).first;

            a.raw_value = neg_a == neg_b ? r : -r;
        }
        return a;
    }


    // template<fixed_point Fxd,
    //          std::convertible_to<Fxd> T>
    // constexpr
    // Fxd&
    // operator %=(Fxd& a,
    //             const T& b)
    //     noexcept
    // {
    //     a.raw_value %= Fxd{b}.raw_value;
    //     return a;
    // }



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


    /*
     * This defines 3 versions of a binary operator:
     * - fixed op fixed
     * - fixed op primitive
     * - primitive op fixed
     */

#define BINOP(op)                               \
    template<fixed_point Fxd>                   \
    constexpr                                   \
    Fxd                                         \
    operator op(Fxd a,                          \
                const Fxd& b)                   \
        noexcept                                \
    {                                           \
        return a op ## = b;                     \
    }                                           \
                                                \
    template<fixed_point Fxd,                   \
             std::convertible_to<Fxd> T>        \
    constexpr                                   \
    Fxd                                         \
    operator op(const Fxd& a,                   \
                const T& b)                     \
        noexcept                                \
    {                                           \
        return a op Fxd{b};                     \
    }                                           \
                                                \
    template<fixed_point Fxd,                   \
             std::convertible_to<Fxd> T>        \
    constexpr                                   \
    Fxd                                         \
    operator op(const T& a,                     \
                const Fxd& b)                   \
        noexcept                                \
    {                                           \
        return Fxd{a} op b;                     \
    }


    BINOP(+)
    BINOP(-)
    BINOP(*)
    BINOP(/)
    BINOP(%)

#undef BINOP



    // ---------- //
    // Comparison //
    // ---------- //


    template<fixed_point Fxd>
    constexpr
    bool
    operator ==(const Fxd& a,
                const Fxd& b)
        noexcept
    {
        return a.raw_value == b.raw_value;
    }


    template<fixed_point Fxd>
    constexpr
    std::strong_ordering
    operator <=>(const Fxd& a,
                 const Fxd& b)
        noexcept
    {
        return a.raw_value <=> b.raw_value;
    }



    // --- //
    // I/O //
    // --- //


    template<fixed_point Fxd>
    std::ostream&
    operator <<(std::ostream& out,
                const Fxd& f)
    {
        if constexpr (Fxd::bits <= std::numeric_limits<float>::digits)
            return out << static_cast<float>(f);
        if constexpr (Fxd::bits <= std::numeric_limits<double>::digits)
            return out << static_cast<double>(f);
        return out << static_cast<long double>(f);
    }






}


#endif
