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
#include <tuple>
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


} // namespace fxd::utils::overflow

#endif
