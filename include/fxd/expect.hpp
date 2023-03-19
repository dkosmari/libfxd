/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_EXPECT_HPP
#define LIBFXD_EXPECT_HPP

#include <concepts>

#include "concepts.hpp"
#include "error.hpp"

#include "detail/safe.hpp"
#include "detail/safe-div.hpp"
#include "detail/safe-mul.hpp"
#include "detail/types.hpp"


/// Return `fxd::expect::maybe<...>` result.
namespace fxd::expect {

    /// Alias to `std::expected`, or an equivalent implementation.
    template<typename T>
    using maybe = detail::expected<T, error>;


    using detail::safe::from_raw;

    using detail::safe::make_fixed;

    /// Convenience overload.
    template<int Int, int Frac,
             typename Raw = detail::select_int_t<Int + Frac>,
             std::convertible_to<fixed<Int, Frac, Raw>> Src>
    constexpr
    maybe<fixed<Int, Frac, Raw>>
    make_fixed(Src src)
    {
        using Dst = fixed<Int, Frac, Raw>;
        return expect::make_fixed<Dst>(src);
    }


    /// Convenience overload (unsigned version).
    template<int Int, int Frac,
             typename Raw = detail::select_uint_t<Int + Frac>,
             std::convertible_to<fixed<Int, Frac, Raw>> Src>
    constexpr
    maybe<fixed<Int, Frac, Raw>>
    make_ufixed(Src src)
    {
        using Dst = fixed<Int, Frac, Raw>;
        return expect::make_fixed<Dst>(src);
    }


    using detail::safe::fixed_cast;


    /// Convenience overload.
    template<int Int, int Frac,
             typename Raw = fxd::detail::select_int_t<Int + Frac>,
             fixed_point Src>
    [[nodiscard]]
    constexpr
    maybe<fixed<Int, Frac, Raw>>
    fixed_cast(Src src)
        noexcept
    {
        using Dst = fixed<Int, Frac, Raw>;
        return expect::fixed_cast<Dst>(src);
    }


    /// Convenience overload (unsigned version).
    template<int Int, int Frac,
             typename Raw = fxd::detail::select_uint_t<Int + Frac>,
             fixed_point Src>
    [[nodiscard]]
    constexpr
    maybe<fixed<Int, Frac, Raw>>
    ufixed_cast(Src src)
        noexcept
    {
        using Dst = fixed<Int, Frac, Raw>;
        return expect::fixed_cast<Dst>(src);
    }


    using detail::safe::to_int;


    /**
     * @brief Assignment.
     *
     * @note This does not return a reference to the argument, only a copy.
     */
    template<fixed_point Dst,
             std::convertible_to<Dst> Src>
    constexpr
    maybe<Dst>
    assign(Dst& dst,
           Src src)
        noexcept
    {
        auto result = expect::make_fixed<Dst>(src);
        if (!result)
            return result;
        return dst = *result;
    }


    /**
     * @brief Pre-increment (`++f`).
     *
     * @note This does not return a reference to the argument, only a copy.
     */
    template<fixed_point Fxd>
    constexpr
    maybe<Fxd>
    pre_inc(Fxd& f)
        noexcept
    {
        auto result = detail::safe::incremented(f);
        if (!result)
            return result;
        return f = *result;
    }


    /// Post-increment (`f++`).
    template<fixed_point Fxd>
    constexpr
    maybe<Fxd>
    post_inc(Fxd& f)
        noexcept
    {
        Fxd old_f = f;
        auto result = expect::pre_inc(f);
        if (!result)
            return result;
        return old_f;
    }


    /**
     * @brief Pre-decrement (`--f`).
     *
     * @note This does not return a reference to the argument, only a copy.
     */
    template<fixed_point Fxd>
    constexpr
    maybe<Fxd>
    pre_dec(Fxd& f)
        noexcept
    {
        auto result = detail::safe::decremented(f);
        if (!result)
            return result;
        return f = *result;
    }


    /// Post-decrement (`f--`).
    template<fixed_point Fxd>
    constexpr
    maybe<Fxd>
    post_dec(Fxd& f)
        noexcept
    {
        Fxd old_f = f;
        auto result = expect::pre_dec(f);
        if (!result)
            return result;
        return old_f;
    }


    using detail::safe::negate;

    using detail::safe::add;

    using detail::safe::sub;

    /// Round down.
    namespace down {

        using detail::safe::down::div;

        using detail::safe::down::mul;

    } // namespace down


    /// Round up.
    namespace up {

        using detail::safe::up::div;

        using detail::safe::up::mul;

    } // namespace up


    /// Round to zero.
    inline
    namespace zero {

        using detail::safe::zero::div;

        using detail::safe::zero::mul;

    } // namespace zero


    using detail::safe::abs;

}


#endif
