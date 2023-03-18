/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_DETAIL_SUB_HPP
#define LIBFXD_DETAIL_SUB_HPP

#include <concepts>
#include <limits>
#include <type_traits>
#include <utility>


namespace fxd::detail::overflow {

#ifdef __amd64__
#define LIBFXD_HAVE_SUB_OVERFLOW_ASM

    template<std::unsigned_integral U>
    requires (type_width<U> <= 64)
    std::pair<U, bool>
    sub_asm(U a,
            U b,
            bool borrow)
        noexcept
    {
        asm("\n\
        addb    $-1, %[BORROW] \n\
        sbb     %[B], %[A] \n\
        setc    %[BORROW] \n\
        "
            : // outputs
              [A] "+R" (a),
              [BORROW] "+&q" (borrow)
            : // inputs
              [B] "rm" (b)
            : // clobber
              "cc"
            );

        return { a, borrow };
    }


    template<std::signed_integral S>
    requires (type_width<S> <= 64)
    std::pair<S, bool>
    sub_asm(S a,
            S b,
            bool borrow)
        noexcept
    {
        asm("\n\
        addb    $-1, %[BORROW] \n\
        sbb     %[B], %[A] \n\
        seto    %[BORROW] \n\
        "
            : // outputs
              [A] "+R" (a),
              [BORROW] "+&q" (borrow)
            : // inputs
              [B] "rm" (b)
            : // clobber
              "cc"
            );

        return { a, borrow };
    }

#endif


#if !defined(LIBFXD_HAVE_SUB_OVERFLOW) \
    && (defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER))
#define LIBFXD_HAVE_SUB_OVERFLOW

    template<std::integral I>
    constexpr
    std::pair<I, bool>
    sub(I a,
        I b,
        bool borrow)
        noexcept
    {
#ifdef LIBFXD_HAVE_SUB_OVERFLOW_ASM
            if (!std::is_constant_evaluated())
                if constexpr (requires { sub_asm<I>(a, b, borrow); })
                    return sub_asm(a, b, borrow);
#endif
        I r1;
        const bool o1 = __builtin_sub_overflow(a, b, &r1);
        I r2;
        const bool o2 = __builtin_sub_overflow(r1, I{borrow}, &r2);
        return { r2, o1 || o2 };
    }


    template<std::integral I>
    constexpr
    std::pair<I, bool>
    sub(I a,
        I b)
        noexcept
    {
        I result;
        const bool ovf = __builtin_sub_overflow(a, b, &result);
        return { result, ovf };
    }

#endif


#ifndef LIBFXD_HAVE_SUB_OVERFLOW
#define LIBFXD_HAVE_SUB_OVERFLOW

    // fallback implementation using only standard language constructs

    template<std::unsigned_integral U>
    constexpr
    std::pair<U, bool>
    sub(U a,
        U b,
        bool borrow = false)
        noexcept
    {
        const U result = a - b - borrow;
        if (a < b)
            return { result, true };
        if (borrow && a == b)
            return { result, true };
        return { result, false };
    }


    template<std::signed_integral S>
    constexpr
    std::pair<S, bool>
    sub(S a,
        S b,
        bool borrow = false)
        noexcept
    {
        constexpr S max = std::numeric_limits<S>::max();
        constexpr S min = std::numeric_limits<S>::min();

        using U = std::make_unsigned_t<S>;
        const U ua = a;
        const U ub = b;
        const U uab = ua - ub;
        const U uresult = ua - ub - borrow;
        const S ab = uab;
        const S result = uresult;
        // a - b < min  <->  a < min + b
        bool ovf = false;
        if (b > 0 && a < min + b)
            ovf = true;
        // a - b > max  <->  a > max + b
        if (b < 0 && a > max + b)
            ovf = true;
        if (ab == min && borrow)
            ovf = true;
        return { result, ovf };
    }


#endif


} // namespace fxd::detail::overflow

#endif
