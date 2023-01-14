#ifndef LIBFXD_UTILS_DIV_HPP
#define LIBFXD_UTILS_DIV_HPP

#include <algorithm>
#include <concepts>
#include <tuple>
#include <utility>

#include "types.hpp"


namespace fxd::utils::div {


    template<std::integral I>
    requires (has_wider_v<I>)
    ALWAYS_INLINE
    constexpr
    wider_t<I>
    div(I a,
        I b)
    {
        constexpr int w = type_width<I>;
        using W = wider_t<I>;
        const W aa = a;
        const W bb = b;
        return (aa << w) / bb;
    }


    template<std::unsigned_integral U>
    constexpr
    std::pair<U, bool>
    shl_overflow(U a,
                 int b)
        noexcept
    {
        constexpr int w = type_width<U>;
        const bool ovf = a >> (w - b);
        const U result = a << b;
        return { result, ovf };
    }


    template<int frac_bits,
             std::unsigned_integral U>
    requires (!has_wider_v<U>)
    std::tuple<U, U, U>
    div(U a,
        U b)
        noexcept
    {
        constexpr int w = type_width<U>;

        U q_hi = a / b;
        U rem = a % b;

        U q_mi = 0;
        for (int i = 0; rem && i < std::min(w, frac_bits); ++i) {

            auto [new_rem, carry] = shl_overflow(rem, 1);
            rem = new_rem;

            if (carry || rem >= b) {
                rem -= b;
                q_mi |= U{1} << (w - i - 1);
            }
        }

        U q_lo = 0;
        if constexpr (frac_bits > w) {
            for (int i = 0; rem && i < frac_bits - w; ++i) {
                auto [new_rem, carry] = shl_overflow(rem, 1);
                rem = new_rem;

                if (carry || rem >= b) {
                    rem -= b;
                    q_lo |= U{1} << (w - i - 1);
                }
            }
        }

        return { q_lo, q_mi, q_hi };
    }


}


#endif
