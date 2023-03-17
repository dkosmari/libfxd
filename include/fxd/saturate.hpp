/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_SATURATE_HPP
#define LIBFXD_SATURATE_HPP

#include <csignal>

#include "concepts.hpp"

#include "impl/error.hpp"
#include "impl/safe-includes.hpp"


/// Clamp on overflow.
namespace fxd::saturate {


    /**
     * @brief Error handler that clamps the value on error.
     *
     * All functions in `fxd::saturate` that return `fxd::fixed` use this to handle
     * errors. The returned value will always be clamped between
     * `std::numeric_limits<Fxd>::lowest()` and `std::numeric_limits<Fxd>::max()` when it
     * makes sense.  The operation `0/0` will not return a value, it will raise the
     * `SIGFPE` signal.
     */
    template<fxd::fixed_point Fxd>
    [[nodiscard]]
    Fxd
    handler(impl::error e)
        noexcept
    {
        switch (e) {
            case impl::error::underflow:
                return std::numeric_limits<Fxd>::lowest();
            case impl::error::overflow:
                return std::numeric_limits<Fxd>::max();
            case impl::error::not_a_number:
                std::raise(SIGFPE);
        }
        return 0;
    }

    /**
     * @brief Error handler that clamps the value on error.
     *
     * All functions in `fxd::saturate` that return integers use this to handle errors.
     * The returned value will always be clamped between `std::numeric_limits<I>::min()`
     * and `std::numeric_limits<I>::max()` when it makes sense, or raise `SIGFPE`
     * otherwise.
     */
    template<std::integral I>
    [[nodiscard]]
    I
    handler(impl::error e)
    {
        switch (e) {
            case impl::error::underflow:
                return std::numeric_limits<I>::min();
            case impl::error::overflow:
                return std::numeric_limits<I>::max();
            case impl::error::not_a_number:
                std::raise(SIGFPE);
        }
        return 0;
    }


#define LIBFXD_INCLUDING_IMPL_SAFE_HPP
#define LIBFXD_HERE saturate
#include "impl/safe.hpp"
#undef LIBFXD_HERE
#undef LIBFXD_INCLUDING_IMPL_SAFE_HPP


#ifdef LIBFXD_DOXYGEN_DOCUMENTATION

    /**
     * @brief Construct from raw value, clamp on overflow.
     *
     * Overflow happens if the represented value does not convert back to the argument's
     * value.
     */
    template<fixed_point Fxd, std::integral I>
    constexpr Fxd from_raw(I val);

    /// Construct from identical fixed-point type.
    template<fixed_point Fxd>
    constexpr Fxd make_fixed(Fxd src) noexcept;

    /// Construct from integer, clamp on overflow.
    template<fixed_point Fxd, std::integral I>
    requires (Fxd::frac_bits < 0)
    constexpr Fxd make_fixed(I val);

    /// Construct from floating-point, clamp on overflow.
    template<fixed_point Fxd, std::floating_point Flt>
    Fxd make_fixed(Flt val);

    /// Construct ufixed, clamp on overflow.
    template<fxd::unsigned_fixed_point Fxd, std::convertible_to<Fxd> Src>
    Fxd make_ufixed(Src src);

    /// Convenience overload.
    template<int Int, int Frac,
             typename I = impl::select_int_t<Int + Frac>,
             std::convertible_to<fixed<Int, Frac, I>> Src>
    constexpr fixed<Int, Frac, I> make_fixed(Src src);

    /// Convenience overload (unsigned version).
    template<int Int,
             int Frac,
             typename U = impl::select_uint_t<Int + Frac>,
             std::convertible_to<fixed<Int, Frac, U>> Src>
    constexpr fixed<Int, Frac, U> make_ufixed(Src src);

    /// Convert between fxd::fixed types, clamp on overflow.
    template<fixed_point Dst, fixed_point Src>
    constexpr Dst fixed_cast(Src src);

    /// Convert between fxd::fixed types, clamp on overflow.
    template<unsigned_fixed_point Dst, fixed_point Src>
    constexpr Dst ufixed_cast(Src src);

    /// Convenience overload.
    template<int Int, int Frac,
             typename Raw = impl::select_int_t<Int + Frac>,
             fixed_point Src>
    constexpr fixed<Int, Frac, Raw> fixed_cast(Src src);

    /// Convenience overload (unsigned version).
    template<int Int, int Frac,
             typename Raw = impl::select_uint_t<Int + Frac>,
             fixed_point Src>
    constexpr fixed<Int, Frac, Raw> ufixed_cast(Src src);

    /// Convert to integer, clamp on overflow.
    template<std::integral I, fixed_point Fxd>
    I to_int(Fxd f);

    /// Convert to the natural integer type, clamp on overflow.
    template<fixed_point Fxd>
    constexpr impl::select_int_for<Fxd::int_bits, typename Fxd::raw_type>
    to_int(Fxd f);

    /// Assignment, clamp on overflow.
    template<fixed_point Fxd, std::convertible_to<Fxd> Src>
    constexpr Fxd& assign(Fxd& dst, Src src);

    /// Pre-increment, clamp on overflow.
    template<fixed_point Fxd>
    constexpr Fxd& inc(Fxd& f);

    /// Post-increment, clamp on overflow.
    template<fixed_point Fxd>
    constexpr Fxd post_inc(Fxd& f);

    /// Pre-decrement, clamp on overflow.
    template<fixed_point Fxd>
    constexpr Fxd& dec(Fxd& f);

    /// Post-decrement, clamp on overflow.
    template<fixed_point Fxd>
    constexpr Fxd post_dec(Fxd& f);

    /// Negate, clamp on overflow.
    template<fixed_point Fxd>
    constexpr Fxd negate(Fxd f);

    /// Add, clamp on overflow.
    template<fixed_point Fxd>
    constexpr Fxd add(Fxd a, Fxd b);

    /// Subtract, clamp on overflow.
    template<fixed_point Fxd>
    constexpr Fxd sub(Fxd a, Fxd b);

    /// Round to zero, and clamp on overflow.
    namespace zero {

        /// Divide rounding to zero, clamp on overflow.
        template<fixed_point Fxd>
        constexpr Fxd div(Fxd a, Fxd b);

        /// Multiply rounding to zero, clamp on overflow.
        template<fixed_point Fxd>
        constexpr Fxd mul(Fxd a, Fxd b);

    }

    /// Round up, and clamp on overflow.
    namespace up {

        /// Divide rounding up, clamp on overflow.
        template<fixed_point Fxd>
        constexpr Fxd div(Fxd a, Fxd b);

        /// Multiply rounding up, clamp on overflow.
        template<fixed_point Fxd>
        constexpr Fxd mul(Fxd a, Fxd b);

    }


    /// Round down, and clamp on overflow.
    namespace down {

        /// Divide rounding down, clamp on overflow.
        template<fixed_point Fxd>
        constexpr Fxd div(Fxd a, Fxd b);

        /// Multiply rounding down, clamp on overflow.
        template<fixed_point Fxd>
        constexpr Fxd mul(Fxd a, Fxd b);

    }



    /// Same as `fxd::abs()`, clamp on overflow.
    template<fixed_point Fxd>
    constexpr Fxd abs(Fxd f);

    /// Same as `fxd::fdim()`, clamp on overflow.
    template<fixed_point Fxd>
    constexpr Fxd fdim(Fxd a, Fxd b);

    /// Same as `fxd::nextafter()`, clamp on overflow.
    template<fixed_point Fxd>
    constexpr Fxd nextafter(Fxd from, Fxd to);


#endif // LIBFXD_DOXYGEN_DOCUMENTATION


} // namespace fxd::saturate


#endif
