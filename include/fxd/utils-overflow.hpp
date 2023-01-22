/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_UTILS_OVERFLOW_HPP
#define LIBFXD_UTILS_OVERFLOW_HPP

#include <concepts>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>


/*
 * TODO: figure out a valid check for C23's <stdckdint.h>
 * It should have precedence over all other methods.
 */
#if 0

#  include <stdckdint.h>
#  define LIBFXD_C23_BUILTINS

#elif defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER)

#  define LIBFXD_GCC_BUILTINS

#elif defined(_MSC_VER) && defined(_M_AMD64)
// Note: MSVC only has the Intel built-ins, and they can't handle negatives
#  if 0
#    include <intrin.h>
#    define LIBFXD_INTEL_BUILTINS
#  else
#    define LIBFXD_NO_BUILTINS
#  endif

#else

#  define LIBFXD_NO_BUILTINS

#endif


namespace fxd::utils::overflow {


#ifdef LIBFXD_C23_BUILTINS

    template<std::integral I>
    constexpr
    std::pair<I, bool>
    add(I a, I b, bool carry = false)
        noexcept
    {
        I result;
        bool ovf = ckd_add(&result, a, b);
        if (carry)
            ovf |= ckd_add(&result, result, I{1});
        return { result, ovf };
    }


    template<std::integral I1>
    constexpr
    std::pair<I, bool>
    sub(I a, I b, bool borrow = false)
        noexcept
    {
        I result;
        bool ovf = ckd_sub(&result, a, b);
        if (borrow)
            ovf |= ckd_sub(&result, result, I{1});
        return { result, ovf };
    }

#endif // LIBFXD_C23_BUILTINS


#ifdef LIBFXD_GCC_BUILTINS

    template<std::integral I>
    constexpr
    std::pair<I, bool>
    add(I a, I b, bool carry = false)
        noexcept
    {
        I result;
        bool ovf = __builtin_add_overflow(a, b, &result);
        if (carry)
            ovf |= __builtin_add_overflow(result, I{1}, &result);
        return { result, ovf };
    }


    template<std::integral I>
    constexpr
    std::pair<I, bool>
    sub(I a, I b, bool borrow = false)
        noexcept
    {
        I result;
        bool ovf = __builtin_sub_overflow(a, b, &result);
        if (borrow)
            ovf |= __builtin_sub_overflow(result, I{1}, &result);
        return { result, ovf };
    }

#endif // LIBFXD_GCC_BUILTINS


#ifdef LIBFXD_INTEL_BUILTINS


    // convenience overloads to dispatch to the correct intrinsic

#define LIBFXD_MAKE_OVERLOAD(sz)                        \
    inline constexpr                                    \
    bool                                                \
    addcarry(bool carry,                                \
             std::uint ## sz ## _t a,                   \
             std::uint ## sz ## _t b,                   \
             std::uint ## sz ##_t* result)              \
        noexcept                                        \
    { return _addcarry_u ## sz (carry, a, b, result); }

    LIBFXD_MAKE_OVERLOAD(8)
    LIBFXD_MAKE_OVERLOAD(16)
    LIBFXD_MAKE_OVERLOAD(32)
    LIBFXD_MAKE_OVERLOAD(64)

#undef LIBFXD_MAKE_OVERLOAD

    template<std::unsigned_integral U>
    constexpr
    std::pair<U, bool>
    add(U a, U b, bool carry = false)
        noexcept
    {
        U result;
        const bool ovf = addcarry(carry, a, b, &result);
        return { result, ovf };
    }


    // convenience overloads to dispatch to the correct intrinsic

#define LIBFXD_MAKE_OVERLOAD(sz)                                \
    inline constexpr                                            \
    bool                                                        \
    subborrow(bool borrow,                                      \
              std::uint ## sz ## _t a,                          \
              std::uint ## sz ## _t b,                          \
              std::uint ## sz ##_t* result)                     \
        noexcept                                                \
    { return _subborrow_u ## sz (borrow, a, b, result); }

    LIBFXD_MAKE_OVERLOAD(8)
    LIBFXD_MAKE_OVERLOAD(16)
    LIBFXD_MAKE_OVERLOAD(32)
    LIBFXD_MAKE_OVERLOAD(64)

#undef LIBFXD_MAKE_OVERLOAD

    template<std::unsigned_integral U>
    constexpr
    std::pair<U, bool>
    sub(U a, U b, bool borrow = false)
        noexcept
    {
        U result;
        const bool ovf = subborrow(borrow, a, b, &result);
        return { result, ovf };
    }


    // TODO: Intel intrinsics don't handle signed integers


#endif // LIBFXD_INTEL_BUILTINS


#ifdef LIBFXD_NO_BUILTINS

    template<std::unsigned_integral U>
    constexpr
    std::pair<U, bool>
    add(U a, U b, bool carry = false)
        noexcept
    {
        const U ab = a + b;
        const U result = a + b + carry;
        constexpr U max = std::numeric_limits<U>::max();
        const bool ovf = (a > max - b) || (ab > max - carry);
        return { result, ovf };
    }


    template<std::signed_integral S>
    constexpr
    std::pair<S, bool>
    add(S a, S b, bool carry = false)
        noexcept
    {
        constexpr S max = std::numeric_limits<S>::max();
        constexpr S min = std::numeric_limits<S>::min();
        using U = std::make_unsigned_t<S>;
        const S ab  = static_cast<U>(a) + static_cast<U>(b);
        const S result = static_cast<U>(a) + static_cast<U>(b) + static_cast<U>(carry);
        const bool ovf =
            (b > 0 && a > max - b) // a + b > max
            ||
            (b < 0 && a < min - b) // a + b < min
            ||
            (c && ab > max - c); // a + b + c > max
        return { result, ovf };
    }

#endif // LIBFXD_NO_BUILTINS



    template<std::unsigned_integral U>
    constexpr
    std::pair<U, bool>
    shl_real(U a,
             unsigned b)
        noexcept
    {
        constexpr int w = type_width<U>;
        if (b >= w)
            return { 0, a != 0 };
        const bool ovf = a >> (w - b);
        const U result = a << b;
        return { result, ovf };
    }


    template<std::unsigned_integral U>
    constexpr
    std::pair<U, bool>
    shr_real(U a,
             unsigned b)
        noexcept
    {
        constexpr int w = type_width<U>;
        if (b >= w)
            return { 0, a != 0 };

        const bool ovf = a << (w - b);
        const U result = a >> b;
        return { result, ovf };
    }


} // namespace fxd::utils::overflow

#endif
