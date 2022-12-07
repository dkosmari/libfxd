#ifndef LIBFXD_LIMITS_HPP
#define LIBFXD_LIMITS_HPP

#include <limits>

#include "fixed.hpp"


namespace std {

    // Note: these are analogous to floating point

    template<int Int,
             int Frac,
             typename T>
    struct numeric_limits<fxd::fixed<Int, Frac, T>> {


        static constexpr bool is_specialized = true;
        static constexpr bool is_signed = numeric_limits<T>::is_signed;
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
        static constexpr bool is_modulo = numeric_limits<T>::is_modulo;
        static constexpr int digits = fxd::fixed<Int, Frac, T>::bits - is_signed;
        // TODO: check if these are correct
        static constexpr int digits10 = (digits - 1) * log10(numeric_limits<T>::radix);
        static constexpr int max_digits10 = ceil(digits * log10(numeric_limits<T>::radix) + 1);

        static constexpr int radix = numeric_limits<T>::radix;
        static constexpr int min_exponent = 0;
        static constexpr int min_exponent10 = 0;
        static constexpr int max_exponent = 0;
        static constexpr int max_exponent10 = 0;
        static constexpr bool traps = numeric_limits<T>::traps;
        static constexpr bool tinyness_before = false;




        // smallest positive value
        static constexpr
        fxd::fixed<Int, Frac, T>
        min()
            noexcept
        {
            return fxd::fixed<Int, Frac, T>::from_raw(1);
        }


        // this is the closest value to -infinity
        static constexpr
        fxd::fixed<Int, Frac, T>
        lowest()
            noexcept
        {
            using Fxd = fxd::fixed<Int, Frac, T>;
            if constexpr (is_signed)
                return Fxd::from_raw(T{1} << (Fxd::bits - 1));
            else
                return Fxd::from_raw(0);
        }


        static constexpr
        fxd::fixed<Int, Frac, T>
        max()
            noexcept
        {
            using Fxd = fxd::fixed<Int, Frac, T>;
            if constexpr (is_signed) {
                using U = make_unsigned_t<T>;
                return Fxd::from_raw((U{1} << (Fxd::bits - 1)) - U{1});
            } else
                return Fxd::from_raw(~T{0});
        }


        static constexpr
        fxd::fixed<Int, Frac, T>
        epsilon()
            noexcept
        {
            return fxd::fixed<Int, Frac, T>::from_raw(1);
        }


        static constexpr
        fxd::fixed<Int, Frac, T>
        round_error()
            noexcept
        {
            return 1;
        }


        static constexpr
        fxd::fixed<Int, Frac, T>
        infinity()
            noexcept
        {
            return 0;
        }


        static constexpr
        fxd::fixed<Int, Frac, T>
        quiet_NaN()
            noexcept
        {
            return 0;
        }


        static constexpr
        fxd::fixed<Int, Frac, T>
        signaling_NaN()
            noexcept
        {
            return 0;
        }


        static constexpr
        fxd::fixed<Int, Frac, T>
        denorm_min()
            noexcept
        {
            return min();
        }


    };


}


#endif
