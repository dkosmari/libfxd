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

#include "fixed.hpp"


#define LIBFXD_LOG10_2(x)      ((x) * 643L / 2136)
#define LIBFXD_LOG10_2_CEIL(x) (((x) * 643L + 2135) / 2136)

namespace std {

    // Note: these are analogous to floating point

    template<int Int,
             int Frac,
             typename Raw>
    requires (numeric_limits<Raw>::is_specialized)
    struct numeric_limits<fxd::fixed<Int, Frac, Raw>> {

        static_assert(numeric_limits<Raw>::radix == 2);
        static_assert(Frac < 2136); // the approximations fail after this

        static constexpr bool is_specialized = true;
        static constexpr bool is_signed = numeric_limits<Raw>::is_signed;
        static constexpr bool is_integer = false;
        static constexpr bool is_exact = false;
        static constexpr bool has_infinity = false;
        static constexpr bool has_quiet_NaN = false;
        static constexpr bool has_signaling_NaN = false;
        static constexpr float_denorm_style has_denorm = denorm_absent;
        static constexpr bool has_denorm_loss = false;
        static constexpr float_round_style round_style = round_toward_zero;
        static constexpr bool is_iec559 = false;
        static constexpr bool is_bounded = true;
        static constexpr bool is_modulo = numeric_limits<Raw>::is_modulo;
        static constexpr int radix = numeric_limits<Raw>::radix;
        static constexpr int digits = fxd::fixed<Int, Frac, Raw>::bits - is_signed;
        static constexpr int digits10 = LIBFXD_LOG10_2(Frac - 1);
        static constexpr int max_digits10 =  max<int>(0, 1 + LIBFXD_LOG10_2_CEIL(Frac));

        static constexpr int min_exponent = 1 - Frac;
        static constexpr int min_exponent10 = LIBFXD_LOG10_2(min_exponent);
        static constexpr int max_exponent = Int - is_signed;
        static constexpr int max_exponent10 = LIBFXD_LOG10_2(max_exponent);
        static constexpr bool traps = numeric_limits<Raw>::traps;
        static constexpr bool tinyness_before = false;




        // smallest positive value
        static constexpr
        fxd::fixed<Int, Frac, Raw>
        min()
            noexcept
        {
            return fxd::fixed<Int, Frac, Raw>::from_raw(1);
        }


        // this is the closest value to -infinity
        static constexpr
        fxd::fixed<Int, Frac, Raw>
        lowest()
            noexcept
        {
            using Fxd = fxd::fixed<Int, Frac, Raw>;
            if constexpr (is_signed)
                return Fxd::from_raw(Raw{1} << (Fxd::bits - 1));
            else
                return Fxd::from_raw(0);
        }


        static constexpr
        fxd::fixed<Int, Frac, Raw>
        max()
            noexcept
        {
            using Fxd = fxd::fixed<Int, Frac, Raw>;
            if constexpr (is_signed) {
                using U = make_unsigned_t<Raw>;
                return Fxd::from_raw((U{1} << (Fxd::bits - 1)) - U{1});
            } else
                return Fxd::from_raw(~Raw{0});
        }


        static constexpr
        fxd::fixed<Int, Frac, Raw>
        epsilon()
            noexcept
        {
            return fxd::fixed<Int, Frac, Raw>::from_raw(1);
        }


        static constexpr
        fxd::fixed<Int, Frac, Raw>
        round_error()
            noexcept
        {
            return epsilon();
        }


        static constexpr
        fxd::fixed<Int, Frac, Raw>
        infinity()
            noexcept
        {
            return 0;
        }


        static constexpr
        fxd::fixed<Int, Frac, Raw>
        quiet_NaN()
            noexcept
        {
            return 0;
        }


        static constexpr
        fxd::fixed<Int, Frac, Raw>
        signaling_NaN()
            noexcept
        {
            return 0;
        }


        static constexpr
        fxd::fixed<Int, Frac, Raw>
        denorm_min()
            noexcept
        {
            return min();
        }


    };


}


#undef LIBFXD_LOG10_2
#undef LIBFXD_LOG10_2_CEIL


#endif
