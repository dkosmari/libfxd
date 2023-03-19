/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_SATURATE_HPP
#define LIBFXD_SATURATE_HPP

#include <concepts>
#include <csignal>

#include "concepts.hpp"
#include "error.hpp"

#include "detail/safe.hpp"
#include "detail/safe-div.hpp"
#include "detail/safe-mul.hpp"


/// Clamp on overflow.
namespace fxd::saturate {


    /**
     * @brief Error handler that clamps the value on error.
     *
     * The returned value will always be clamped between
     * `std::numeric_limits<T>::lowest()` and `std::numeric_limits<T>::max()` when it
     * makes sense.  The operation `0/0` will not return a value, it will raise the
     * `SIGFPE` signal.
     */
    template<typename T>
    [[nodiscard]]
    constexpr
    T
    saturate(error e)
        noexcept
    {
        switch (e) {
            case error::underflow:
                return std::numeric_limits<T>::lowest();
            case error::overflow:
                return std::numeric_limits<T>::max();
            case error::not_a_number:
            default:
                std::raise(SIGFPE);
        }
        return 0;
    }


#define WRAP(expr)                                              \
    do {                                                        \
        auto result = detail::safe::expr;                       \
        using Ret = typename decltype(result)::value_type;      \
        if (!result)                                            \
            return saturate<Ret>(result.error());               \
        return *result;                                         \
    } while (false)


    /**
     * @brief Construct from raw value, clamp on overflow.
     *
     * Overflow happens if the represented value does not convert back to the argument's
     * value.
     */
    template<fixed_point Fxd,
             std::integral I>
    [[nodiscard]]
    constexpr
    Fxd
    from_raw(I val)
        noexcept
    {
        WRAP(from_raw<Fxd>(val));
    }


    /// Construct from numerical value, clamp on overflow.
    template<fixed_point Dst,
             std::convertible_to<Dst> Src>
    [[nodiscard]]
    constexpr
    Dst
    make_fixed(Src src)
        noexcept
    {
        WRAP(make_fixed<Dst>(src));
    }


    /// Construct from numerical value, clamp on overflow (unsigned version).
    template<unsigned_fixed_point Dst,
             std::convertible_to<Dst> Src>
    [[nodiscard]]
    constexpr
    Dst
    make_ufixed(Src src)
        noexcept
    {
        WRAP(make_fixed<Dst>(src));
    }


    /// Convenience overload
    template<int Int, int Frac,
             typename Raw = detail::select_int_t<Int + Frac>,
             std::convertible_to<fixed<Int, Frac, Raw>> Src>
    [[nodiscard]]
    constexpr
    fixed<Int, Frac, Raw>
    make_fixed(Src src)
        noexcept
    {
        using Dst = fixed<Int, Frac, Raw>;
        return saturate::make_fixed<Dst>(src);
    }


    /// Convert between `fxd::fixed` types, clamp on overflow.
    template<fixed_point Dst,
             fixed_point Src>
    [[nodiscard]]
    constexpr
    Dst
    fixed_cast(Src src)
        noexcept
    {
        WRAP(fixed_cast<Dst>(src));
    }


    /// Convenience overload.
    template<int Int, int Frac,
             typename Raw = detail::select_int_t<Int + Frac>,
             fixed_point Src>
    [[nodiscard]]
    constexpr
    fixed<Int, Frac, Raw>
    fixed_cast(Src src)
        noexcept
    {
        using Dst = fixed<Int, Frac, Raw>;
        return saturate::fixed_cast<Dst>(src);
    }


    /// Convenience overload (unsigned version).
    template<int Int, int Frac,
             typename Raw = detail::select_uint_t<Int + Frac>,
             fixed_point Src>
    [[nodiscard]]
    constexpr
    fixed<Int, Frac, Raw>
    ufixed_cast(Src src)
        noexcept
    {
        using Dst = fixed<Int, Frac, Raw>;
        return saturate::fixed_cast<Dst>(src);
    }


    /// Convert to integer, clamp on overflow.
    template<std::integral I,
             fixed_point Fxd>
    [[nodiscard]]
    constexpr
    I
    to_int(Fxd f)
        noexcept
    {
        WRAP(to_int<I>(f));
    }


    /// Convert to the natural integer type, clamp on overflow.
    template<fixed_point Fxd>
    [[nodiscard]]
    constexpr
    detail::select_int_for<Fxd::int_bits, typename Fxd::raw_type>
    to_int(Fxd f)
    {
        using I = detail::select_int_for<Fxd::int_bits, typename Fxd::raw_type>;
        return saturate::to_int<I>(f);
    }


    /// Assignment, clamp on overflow.
    template<fixed_point Dst,
             std::convertible_to<Dst> Src>
    constexpr
    Dst&
    assign(Dst& dst,
           Src src)
        noexcept
    {
        return dst = saturate::make_fixed<Dst>(src);
    }


    /// Pre-increment (`++f`), clamp on overflow.
    template<fixed_point Fxd>
    constexpr
    Fxd&
    pre_inc(Fxd& f)
        noexcept
    {
        auto result = detail::safe::incremented(f);
        if (!result)
            return f = saturate<Fxd>(result.error());
        return f = *result;
    }


    /// Post-increment (`f++`), clamp on overflow.
    template<fixed_point Fxd>
    constexpr
    Fxd
    post_inc(Fxd& f)
        noexcept
    {
        Fxd old_f = f;
        saturate::pre_inc(f);
        return old_f;
    }


    /// Pre-decrement (`--f`), clamp on overflow.
    template<fixed_point Fxd>
    constexpr
    Fxd&
    pre_dec(Fxd& f)
        noexcept
    {
        auto result = detail::safe::decremented(f);
        if (!result)
            return f = saturate<Fxd>(result.error());
        return f = *result;
    }


    /// Post-decrement (`f--`), clamp on overflow.
    template<fixed_point Fxd>
    constexpr
    Fxd
    post_dec(Fxd& f)
        noexcept
    {
        Fxd old_f = f;
        saturate::pre_dec(f);
        return old_f;
    }


    /// Negate, clamp on overflow.
    template<fixed_point Fxd>
    [[nodiscard]]
    constexpr
    Fxd
    negate(Fxd f)
        noexcept
    {
        WRAP(negate(f));
    }


    /// Add, clamp on overflow.
    template<fixed_point Fxd>
    [[nodiscard]]
    constexpr
    Fxd
    add(Fxd a,
        Fxd b)
        noexcept
    {
        WRAP(add(a, b));
    }


    /// Subtract, clamp on overflow.
    template<fixed_point Fxd>
    [[nodiscard]]
    constexpr
    Fxd
    sub(Fxd a,
        Fxd b)
        noexcept
    {
        WRAP(sub(a, b));
    }


    /// Round down, and clamp on overflow.
    namespace down {

        /// Divide rounding down, clamp on overflow.
        template<fixed_point Fxd>
        [[nodiscard]]
        constexpr
        Fxd
        div(Fxd a,
            Fxd b)
            noexcept
        {
            WRAP(down::div(a, b));
        }


        /// Multiply rounding down, clamp on overflow.
        template<fixed_point Fxd>
        [[nodiscard]]
        constexpr
        Fxd
        mul(Fxd a,
            Fxd b)
            noexcept
        {
            WRAP(down::mul(a, b));
        }

    } // namespace down


    /// Round up, and clamp on overflow.
    namespace up {

        /// Divide rounding up, clamp on overflow.
        template<fixed_point Fxd>
        [[nodiscard]]
        constexpr
        Fxd
        div(Fxd a,
            Fxd b)
            noexcept
        {
            WRAP(up::div(a, b));
        }


        /// Multiply rounding up, clamp on overflow.
        template<fixed_point Fxd>
        [[nodiscard]]
        constexpr
        Fxd
        mul(Fxd a,
            Fxd b)
            noexcept
        {
            WRAP(up::mul(a, b));
        }

    } // namespace up


    /// Round to zero, and clamp on overflow.
    inline
    namespace zero {

        /// Divide rounding to zero, clamp on overflow.
        template<fixed_point Fxd>
        [[nodiscard]]
        constexpr
        Fxd
        div(Fxd a,
            Fxd b)
            noexcept
        {
            WRAP(zero::div(a, b));
        }

        /// Multiply rounding to zero, clamp on overflow.
        template<fixed_point Fxd>
        [[nodiscard]]
        constexpr
        Fxd
        mul(Fxd a,
            Fxd b)
            noexcept
        {
            WRAP(zero::mul(a, b));
        }

    } // namespace zero


    /// Same as `fxd::abs()`, clamp on overflow.
    template<fixed_point Fxd>
    [[nodiscard]]
    constexpr
    Fxd
    abs(Fxd f)
        noexcept
    {
        WRAP(abs(f));
    }


#undef WRAP


} // namespace fxd::saturate


#endif
