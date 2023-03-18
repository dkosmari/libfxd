/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_DETAIL_MUL_HPP
#define LIBFXD_DETAIL_MUL_HPP

#include <concepts>
#include <tuple>
#include <type_traits> // make_unsigned_t
#include <utility> // tie()

#include "types.hpp"
#include "add.hpp"


namespace fxd::detail {


    template<int bits,
             std::integral I>
    requires (has_int_for<2 * bits, I>)
    constexpr
    select_int_for<2 * bits, I>
    mul(I a,
        I b)
        noexcept
    {
        using II = select_int_for<2 * bits, I>;
        return static_cast<II>(a) * static_cast<II>(b);
    }

    // returns low, high parts of the full multiplication a * b
    // without using larger arithmetic than I
    template<int bits,
             std::integral I>
    requires (!has_int_for<2 * bits, I>)
    constexpr
    std::tuple<std::make_unsigned_t<I>, I>
    mul(I a,
        I b)
        noexcept
    {

#ifdef LIBFXD_PARTIAL_INT128

        using U = std::make_unsigned_t<I>;

        using II = std::conditional_t<std::numeric_limits<I>::is_signed,
                                      int128_t,
                                      uint128_t>;

        static_assert(sizeof(II) > sizeof(I));

        const II cc = static_cast<II>(a) * static_cast<II>(b);

        return {
            static_cast<U>(cc),
            static_cast<I>(cc >> type_width<U>)
        };

#else

        // fallback implementation using only standard C++

        constexpr int k = type_width<I> / 2;
        using U = std::make_unsigned_t<I>;
        constexpr U mask = (U{1} << k) - 1;

        const U a0 = a & mask;
        const U a1 = a >> k; // note: intentional sign extension may happen

        const U b0 = b & mask;
        const U b1 = b >> k; // note: intentional sign extension may happen

        const I a0b1 = a0 * b1;
        const I a1b0 = a1 * b0;

        U c01 = a0 * b0;

        bool carry1, carry2;
        std::tie(c01, carry1) = utils::add::overflow::add(c01,
                                                          static_cast<U>(a0b1 & mask) << k);
        std::tie(c01, carry2) = utils::add::overflow::add(c01,
                                                          static_cast<U>(a1b0 & mask) << k);

        I c23 = a1 * b1
            + static_cast<U>(a0b1 >> k)
            + static_cast<U>(a1b0 >> k)
            + carry1 + carry2;

        return { c01, c23 };

#endif // LIBFXD_PARTIAL_INT128

    }


    namespace overflow {

        template<std::integral I>
        constexpr
        std::pair<I, bool>
        mul(I a,
            I b)
            noexcept
        {
            I res;
            bool ovf = __builtin_mul_overflow(a, b, &res);
            return { res, ovf };
        }
    }


}


#endif
