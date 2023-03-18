/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_LIMITS_HPP
#define LIBFXD_LIMITS_HPP

#include <algorithm>
#include <limits>

#include "constructors.hpp"
#include "fixed.hpp"

#include "detail/types.hpp"


#define LIBFXD_LOG10_2(x)      ((x) * 643L / 2136)
#define LIBFXD_LOG10_2_CEIL(x) (((x) * 643L + 2135) / 2136)

namespace std {

    /// Specialization: fxd::fixed is similar to floating-point.
    template<int Int,
             int Frac,
             typename Raw>
    requires (numeric_limits<Raw>::is_specialized)
    struct numeric_limits<fxd::fixed<Int, Frac, Raw>> {

        static_assert(numeric_limits<Raw>::radix == 2);
        static_assert(Frac < 2136); // the approximations fail after this

        /// This is a valid specialization.
        static constexpr bool is_specialized = true;

        /// Sign depends on the underlying integer type.
        static constexpr bool is_signed = numeric_limits<Raw>::is_signed;

        /// If this represents integers only.
        static constexpr bool is_integer = Frac <= 0;

        /// All arithmetic is done with integers, so `fxd::fixed` is exact.
        static constexpr bool is_exact = true;

        /// No representation for @b ∞ .
        static constexpr bool has_infinity = false;

        /// No NaN.
        static constexpr bool has_quiet_NaN = false;

        /// No NaN.
        static constexpr bool has_signaling_NaN = false;

        /// No denormals.
        static constexpr float_denorm_style has_denorm = denorm_absent;

        /// No denormals.
        static constexpr bool has_denorm_loss = false;

        /// Default rounding is towards zero, just like with integers.
        static constexpr float_round_style round_style = round_toward_zero;

        /// Not a standard floating point type.
        static constexpr bool is_iec559 = false;

        /// It's bounded by the underlying integer type.
        static constexpr bool is_bounded = numeric_limits<Raw>::is_bounded;

        /// It's modulo if the underlying integer is modulo.
        static constexpr bool is_modulo = numeric_limits<Raw>::is_modulo;

        /// Same radix as the underlying integer.
        static constexpr int radix = numeric_limits<Raw>::radix;

        /// `fxd::fixed::bits` - `is_signed`. @hideinitializer
        static constexpr int digits = fxd::fixed<Int, Frac, Raw>::bits - is_signed;

        /// Same estimation used for floating-point. @hideinitializer
        static constexpr int digits10 = LIBFXD_LOG10_2(Frac - 1);

        /// Same estimation used for floating-point. @hideinitializer
        static constexpr int max_digits10 =  max<int>(0, 1 + LIBFXD_LOG10_2_CEIL(Frac));

        /// Minimum power-of-two that can be stored.
        static constexpr int min_exponent = 1 - Frac;

        /// Same estimation used for floating-point. @hideinitializer
        static constexpr int min_exponent10 = LIBFXD_LOG10_2(min_exponent);

        /// Maximum power-of-two that can be stored.
        static constexpr int max_exponent = Int - is_signed;

        /// Same estimation used for floating-point. @hideinitializer
        static constexpr int max_exponent10 = LIBFXD_LOG10_2(max_exponent);

        /// Depends on the underlying integer.
        static constexpr bool traps = numeric_limits<Raw>::traps;

        /// Not meaningful.
        static constexpr bool tinyness_before = false;

        /// Extension: maximum logical bit.
        static constexpr int max_bit = Int - is_signed;

        /// Extension: minimum logical bit.
        static constexpr int min_bit = - Frac;

        /// Extension: lossless conversion to a floating-point type.
        using float_type = fxd::detail::select_float_t<digits>;


        /// Smallest positive value; same semantics as floating-point.
        static constexpr
        fxd::fixed<Int, Frac, Raw>
        min()
            noexcept
        {
            return fxd::fixed<Int, Frac, Raw>::from_raw(1);
        }


        /// Closest value to @b -∞.
        static constexpr
        fxd::fixed<Int, Frac, Raw>
        lowest()
            noexcept
        {
            using Fxd = fxd::fixed<Int, Frac, Raw>;
            if constexpr (is_signed)
                return Fxd::from_raw(Raw{-1} << (Fxd::bits - 1));
            else
                return Fxd::from_raw(0);
        }


        /// Maximum value.
        static constexpr
        fxd::fixed<Int, Frac, Raw>
        max()
            noexcept
        {
            using Fxd = fxd::fixed<Int, Frac, Raw>;
            if constexpr (is_signed) {
                using URaw = make_unsigned_t<Raw>;
                return Fxd::from_raw((URaw{1} << (Int + Frac - 1)) - URaw{1});
            } else
                return Fxd::from_raw(~Raw{0});
        }


        /// Smallest increment.
        static constexpr
        fxd::fixed<Int, Frac, Raw>
        epsilon()
            noexcept
        {
            return fxd::fixed<Int, Frac, Raw>::from_raw(1);
        }


        /// Maximum possible error.
        static constexpr
        fxd::fixed<Int, Frac, Raw>
        round_error()
            noexcept
        {
            return epsilon();
        }


        /// Not meaningful, `has_infinity` is false.
        static constexpr
        fxd::fixed<Int, Frac, Raw>
        infinity()
            noexcept
        {
            return 0;
        }


        /// Not meaningful, `has_quiet_NaN` is false.
        static constexpr
        fxd::fixed<Int, Frac, Raw>
        quiet_NaN()
            noexcept
        {
            return 0;
        }


        /// Not meaningful, `has_signaling_NaN` is false.
        static constexpr
        fxd::fixed<Int, Frac, Raw>
        signaling_NaN()
            noexcept
        {
            return 0;
        }


        /// Same as `min()` since there are no denormalized values.
        static constexpr
        fxd::fixed<Int, Frac, Raw>
        denorm_min()
            noexcept
        {
            return min();
        }


        /// Extension: maximum power of two.
        static constexpr
        fxd::fixed<Int, Frac, Raw>
        max_pow2()
        noexcept
        {
            return fxd::fixed<Int, Frac, Raw>::from_raw(1 << (Int+Frac - is_signed));
        }

    };


}


#undef LIBFXD_LOG10_2
#undef LIBFXD_LOG10_2_CEIL


#endif
