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
#include "impl/error.hpp"
#include "impl/safe-includes.hpp"


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
    template<fxd::fixed_point Fxd>
    [[noreturn]]
    Fxd
    handler(impl::error e)
    {
        switch (e) {
            case impl::error::underflow:
                throw std::underflow_error{"underflow"};
            case impl::error::overflow:
                throw std::overflow_error{"overflow"};
            case impl::error::not_a_number:
                throw std::invalid_argument{"not a number"};
            default:
                throw std::logic_error{"unknown error"};
        }
    }


    /// @copydoc handler()
    template<std::integral I>
    [[noreturn]]
    I
    handler(impl::error e)
    {
        switch (e) {
            case impl::error::underflow:
                throw std::underflow_error{"underflow"};
            case impl::error::overflow:
                throw std::overflow_error{"overflow"};
            case impl::error::not_a_number:
                throw std::invalid_argument{"not a number"};
            default:
                throw std::logic_error{"unknown error"};
        }
    }


#define LIBFXD_INCLUDING_IMPL_SAFE_HPP
#define LIBFXD_HERE except
#include "impl/safe.hpp"
#undef LIBFXD_HERE
#undef LIBFXD_INCLUDING_IMPL_SAFE_HPP



#ifdef LIBFXD_DOXYGEN_DOCUMENTATION

    /**
     * @brief Construct from raw value, throw on overflow.
     *
     * Overflow happens if the represented value does not convert back to the argument's
     * value.
     */
    template<fixed_point Fxd, std::integral I>
    constexpr Fxd from_raw(I val);

    /// Construct from identical fixed-point type.
    template<fixed_point Fxd>
    Fxd make_fixed(Fxd src) noexcept;

    /// Construct from integer, throw on overflow.
    template<fixed_point Fxd, std::integral I>
    requires (Fxd::frac_bits < 0)
    constexpr Fxd make_fixed(I val);

    /// Construct from floating-point, throw on overflow.
    template<fixed_point Fxd, std::floating_point Flt>
    Fxd make_fixed(Flt val);

    /// Construct ufixed, throw on overflow.
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

    /// Convert between fxd::fixed types, throw on overflow.
    template<fixed_point Dst, fixed_point Src>
    constexpr Dst fixed_cast(Src src);

    /// Convert between fxd::fixed types, throw on overflow.
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

    /// Convert to integer, throw on overflow.
    template<std::integral I, fixed_point Fxd>
    I to_int(Fxd f);

    /// Convert to the natural integer type, throw on overflow.
    template<fixed_point Fxd>
    constexpr impl::select_int_for<Fxd::int_bits, typename Fxd::raw_type>
    to_int(Fxd f);

    /// Assignment, throw on overflow.
    template<fixed_point Fxd, std::convertible_to<Fxd> Src>
    constexpr Fxd& assign(Fxd& dst, Src src);

    /// Pre-increment, throw on overflow.
    template<fixed_point Fxd>
    constexpr Fxd& inc(Fxd& f);

    /// Post-increment, throw on overflow.
    template<fixed_point Fxd>
    constexpr Fxd post_inc(Fxd& f);

    /// Pre-decrement, throw on overflow.
    template<fixed_point Fxd>
    constexpr Fxd& dec(Fxd& f);

    /// Post-decrement, throw on overflow.
    template<fixed_point Fxd>
    constexpr Fxd post_dec(Fxd& f);

    /// Negate, throw on overflow.
    template<fixed_point Fxd>
    constexpr Fxd negate(Fxd f);

    /// Add, throw on overflow.
    template<fixed_point Fxd>
    constexpr Fxd add(Fxd a, Fxd b);

    /// Subtract, throw on overflow.
    template<fixed_point Fxd>
    constexpr Fxd sub(Fxd a, Fxd b);

    /// Round to zero, and throw on overflow.
    namespace zero {

        /// Divide rounding to zero, throw on overflow.
        template<fixed_point Fxd>
        constexpr Fxd div(Fxd a, Fxd b);

        /// Multiply rounding to zero, throw on overflow.
        template<fixed_point Fxd>
        constexpr Fxd mul(Fxd a, Fxd b);

    }

    /// Round up, and throw on overflow.
    namespace up {

        /// Divide rounding up, throw on overflow.
        template<fixed_point Fxd>
        constexpr Fxd div(Fxd a, Fxd b);

        /// Multiply rounding up, throw on overflow.
        template<fixed_point Fxd>
        constexpr Fxd mul(Fxd a, Fxd b);

    }


    /// Round down, and throw on overflow.
    namespace down {

        /// Divide rounding down, throw on overflow.
        template<fixed_point Fxd>
        constexpr Fxd div(Fxd a, Fxd b);

        /// Multiply rounding down, throw on overflow.
        template<fixed_point Fxd>
        constexpr Fxd mul(Fxd a, Fxd b);

    }



    /// Same as `fxd::abs()`, throw on overflow.
    template<fixed_point Fxd>
    constexpr Fxd abs(Fxd f);

    /// Same as `fxd::fdim()`, throw on overflow.
    template<fixed_point Fxd>
    constexpr Fxd fdim(Fxd a, Fxd b);

    /// Same as `fxd::nextafter()`, throw on overflow.
    template<fixed_point Fxd>
    constexpr Fxd nextafter(Fxd from, Fxd to);


#endif // LIBFXD_DOXYGEN_DOCUMENTATION


} // namespace fxd::except


#endif
