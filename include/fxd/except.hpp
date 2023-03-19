/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_EXCEPT_HPP
#define LIBFXD_EXCEPT_HPP

#include <concepts>
#include <cstdlib>
#include <stdexcept>

#include "concepts.hpp"
#include "error.hpp"

#include "detail/safe.hpp"
#include "detail/safe-div.hpp"
#include "detail/safe-mul.hpp"


/// Throw on overflow.
namespace fxd::except {


    /**
     * @brief Error handler that throws exceptions on error.
     *
     * All functions in `fxd::except` use this as an error handler.
     * Possible exceptions are:
     * @li `std::underflow_error` on underflows;
     * @li `std::overflow_error` on overflows;
     * @li `std::invalid_argument` when `NaN` would be generated;
     */
    [[noreturn]]
    inline
    void
    except(error e)
    {
        switch (e) {
            case error::underflow:
                throw std::underflow_error{"underflow"};
            case error::overflow:
                throw std::overflow_error{"overflow"};
            case error::not_a_number:
                throw std::invalid_argument{"not a number"};
            default:
                throw std::logic_error{"unknown error"};
        }
    }


#define WRAP(expr)                              \
    do {                                        \
        auto result = detail::safe::expr;       \
        if (!result)                            \
            except(result.error());             \
        return *result;                         \
    } while (false)


    /**
     * @brief Construct from raw value, throw on overflow.
     *
     * Overflow happens if the represented value does not convert back to the argument's
     * value.
     */
    template<fixed_point Fxd,
             std::integral I>
    constexpr
    Fxd
    from_raw(I val)
    {
        WRAP(from_raw<Fxd>(val));
    }


    /// Construct from numerical value, throw on overflow.
    template<fixed_point Dst,
             std::convertible_to<Dst> Src>
    constexpr
    Dst
    make_fixed(Src src)
    {
        WRAP(make_fixed<Dst>(src));
    }


    /// Convenience overload.
    template<int Int, int Frac,
             typename Raw = detail::select_int_t<Int + Frac>,
             std::convertible_to<fixed<Int, Frac, Raw>> Src>
    constexpr
    fixed<Int, Frac, Raw>
    make_fixed(Src src)
    {
        using Dst = fixed<Int, Frac, Raw>;
        return except::make_fixed<Dst>(src);
    }


    /// Convenience overload (unsigned version).
    template<int Int, int Frac,
             typename Raw = detail::select_uint_t<Int + Frac>,
             std::convertible_to<fixed<Int, Frac, Raw>> Src>
    constexpr
    fixed<Int, Frac, Raw>
    make_ufixed(Src src)
    {
        using Dst = fixed<Int, Frac, Raw>;
        return except::make_fixed<Dst>(src);
    }


    /// Convert between `fxd::fixed` types, throw on overflow.
    template<fixed_point Dst,
             fixed_point Src>
    constexpr
    Dst
    fixed_cast(Src src)
    {
        WRAP(fixed_cast<Dst>(src));
    }


    /// Convenience overload.
    template<int Int, int Frac,
             typename Raw = detail::select_int_t<Int + Frac>,
             fixed_point Src>
    constexpr
    fixed<Int, Frac, Raw>
    fixed_cast(Src src)
    {
        using Dst = fixed<Int, Frac, Raw>;
        return except::fixed_cast<Dst>(src);
    }


    /// Convenience overload (unsigned version).
    template<int Int, int Frac,
             typename Raw = detail::select_uint_t<Int + Frac>,
             fixed_point Src>
    constexpr
    fixed<Int, Frac, Raw>
    ufixed_cast(Src src)
    {
        using Dst = fixed<Int, Frac, Raw>;
        return except::fixed_cast<Dst>(src);
    }


    /// Convert to integer, throw on overflow.
    template<std::integral I,
             fixed_point Fxd>
    I
    to_int(Fxd f)
    {
        WRAP(to_int<I>(f));
    }


    /// Convert to the natural integer type, throw on overflow.
    template<fixed_point Fxd>
    constexpr
    detail::select_int_for<Fxd::int_bits, typename Fxd::raw_type>
    to_int(Fxd f)
    {
        using I = detail::select_int_for<Fxd::int_bits, typename Fxd::raw_type>;
        return except::to_int<I>(f);
    }


    /// Assignment, throw on overflow.
    template<fixed_point Dst,
             std::convertible_to<Dst> Src>
    constexpr
    Dst&
    assign(Dst& dst,
           Src src)
    {
        return dst = except::make_fixed<Dst>(src);
    }


    /// Pre-increment (`++f`), throw on overflow.
    template<fixed_point Fxd>
    constexpr
    Fxd&
    pre_inc(Fxd& f)
    {
        auto result = detail::safe::incremented(f);
        if (!result)
            except(result.error());
        return f = *result;
    }


    /// Post-increment (`f++`), throw on overflow.
    template<fixed_point Fxd>
    constexpr
    Fxd
    post_inc(Fxd& f)
    {
        Fxd old_f = f;
        except::pre_inc(f);
        return old_f;
    }


    /// Pre-decrement (`--f`), throw on overflow.
    template<fixed_point Fxd>
    constexpr
    Fxd&
    pre_dec(Fxd& f)
    {
        auto result = detail::safe::decremented(f);
        if (!result)
            except(result.error());
        return f = *result;
    }


    /// Post-decrement (`f--`), throw on overflow.
    template<fixed_point Fxd>
    constexpr
    Fxd
    post_dec(Fxd& f)
    {
        Fxd old_f = f;
        except::pre_dec(f);
        return old_f;
    }


    /// Negate, throw on overflow.
    template<fixed_point Fxd>
    constexpr
    Fxd
    negate(Fxd f)
    {
        WRAP(negate(f));
    }


    /// Add, throw on overflow.
    template<fixed_point Fxd>
    constexpr
    Fxd
    add(Fxd a,
        Fxd b)
    {
        WRAP(add(a, b));
    }


    /// Subtract, throw on overflow.
    template<fixed_point Fxd>
    constexpr
    Fxd
    sub(Fxd a,
        Fxd b)
    {
        WRAP(sub(a, b));
    }


    /// Round down, and throw on overflow.
    namespace down {

        /// Divide rounding down, throw on overflow.
        template<fixed_point Fxd>
        constexpr
        Fxd
        div(Fxd a,
            Fxd b)
        {
            WRAP(down::div(a, b));
        }


        /// Multiply rounding down, throw on overflow.
        template<fixed_point Fxd>
        constexpr
        Fxd
        mul(Fxd a,
            Fxd b)
        {
            WRAP(down::mul(a, b));
        }

    } // namespace down


    /// Round up, and throw on overflow.
    namespace up {

        /// Divide rounding up, throw on overflow.
        template<fixed_point Fxd>
        constexpr
        Fxd
        div(Fxd a,
            Fxd b)
        {
            WRAP(up::div(a, b));
        }


        /// Multiply rounding up, throw on overflow.
        template<fixed_point Fxd>
        constexpr
        Fxd
        mul(Fxd a,
            Fxd b)
        {
            WRAP(up::mul(a, b));
        }

    } // namespace up


    /// Round to zero, and throw on overflow.
    inline
    namespace zero {

        /// Divide rounding to zero, throw on overflow.
        template<fixed_point Fxd>
        constexpr
        Fxd
        div(Fxd a,
            Fxd b)
        {
            WRAP(zero::div(a, b));
        }


        /// Multiply rounding to zero, throw on overflow.
        template<fixed_point Fxd>
        constexpr
        Fxd
        mul(Fxd a,
            Fxd b)
        {
            WRAP(zero::mul(a, b));
        }

    } // namespace zero


    /// Same as `fxd::abs()`, throw on overflow.
    template<fixed_point Fxd>
    constexpr
    Fxd
    abs(Fxd f)
    {
        WRAP(abs(f));
    }


#undef WRAP


} // namespace fxd::except


#endif
