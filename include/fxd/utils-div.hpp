#ifndef LIBFXD_UTILS_DIV_HPP
#define LIBFXD_UTILS_DIV_HPP

#include <algorithm>
#include <bit>
#include <concepts>
#include <optional>
#include <tuple>
#include <utility>

#include "types.hpp"

#include "utils-overflow.hpp"
#include "utils-shift.hpp"


namespace fxd::utils::div {



    // performs a floating-point division
    // returns the mantissa and exponent as a pair
    template<int frac_bits,
             std::unsigned_integral U>
    std::pair<U, int>
    div(U a,
        U b)
        noexcept
    {
        using shift::shl_real;

        if (!a)
            return {0, 0};

        const int expo_a = std::countl_zero(a);
        const int expo_b = std::countl_zero(b);
        // scale of the result
        const int scale = expo_b - expo_a;

        a = shl_real(a, expo_a);
        b = shl_real(b, expo_b);

        U quo = a / b; // calculates the first bit, may also div-by-zero trap
        U rem = a % b;

        int i = 0;
        for (; rem && i < frac_bits + scale; ++i) {

            auto [new_rem, carry] = overflow::shl_real(rem, 1);
            rem = new_rem;

            quo <<= 1;
            if (carry || rem >= b) {
                quo |= 1;
                rem -= b;
            }
        }

        return { quo, scale - i };
    }


    template<int frac_bits,
             std::unsigned_integral U>
    requires (has_wider_v<U> && frac_bits > 0 && frac_bits <= type_width<wider_t<U>>)
    std::pair<U, int>
        div(U a,
            U b)
        noexcept
    {
        using W = wider_t<U>;
        W aa = shift::shl_real<W>(a, frac_bits);
        W bb = b;
        W cc = aa / bb;
        return { cc, -frac_bits };
    }


    template<int frac_bits,
             std::unsigned_integral U>
    requires (frac_bits <= 0)
    std::pair<U, int>
        div(U a,
            U b)
        noexcept
    {
        return { a / b, 0 };
    }



    namespace overflow {

        // same as above, but the result is optional; if it's missing, there was an overflow
        template<int frac_bits,
                 std::unsigned_integral U>
        std::optional<std::pair<U, int>>
        div(U a,
            U b)
            noexcept
        {
            using utils::shift::shl_real;

            if (!a)
                return std::pair{0, 0};

            const int expo_a = std::countl_zero(a);
            const int expo_b = std::countl_zero(b);
            // scale of the result
            const int scale = expo_b - expo_a;

            a = shl_real(a, expo_a);
            b = shl_real(b, expo_b);

            if (!b)
                return {};

            U quo = a / b; // calculates the first bit
            U rem = a % b;

            int i = 0;
            for (; rem && i < frac_bits + scale; ++i) {

                auto [new_rem, carry] = utils::overflow::shl_real(rem, 1);
                rem = new_rem;

                auto [new_quo, ovf] = utils::overflow::shl_real(quo, 1);
                if (ovf)
                    return {};
                quo = new_quo;

                if (carry || rem >= b) {
                    quo |= 1;
                    rem -= b;
                }
            }

            return std::pair{ quo, scale - i };
        }

    }


}


#endif
