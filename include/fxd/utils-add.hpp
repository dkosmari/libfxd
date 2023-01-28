/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_UTILS_ADD_HPP
#define LIBFXD_UTILS_ADD_HPP

#include <concepts>
#include <tuple>
#include <utility>



#if 0
#  include <stdckdint.h>
#  define LIBFXD_C23_BUILTINS
#elif defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER)
#  define LIBFXD_GCC_BUILTINS
#else
#  define LIBFXD_NO_BUILTINS
#endif


#include "utils-overflow.hpp"
#include "utils-tuple.hpp"


namespace fxd::utils::add {


    namespace overflow {

#ifdef LIBFXD_C23_BUILTINS
        template<std::integral I>
        constexpr
        std::pair<I, bool>
        add(I a,
            I b,
            bool carry = false)
            noexcept
        {
            I result;
            bool ovf = ckd_add(&result, a, b);
            if (carry)
                ovf |= ckd_add(&result, result, I{1});
            return { result, ovf };
        }
#endif

#ifdef LIBFXD_GCC_BUILTINS
        template<std::integral I>
        constexpr
        std::pair<I, bool>
        add(I a,
            I b,
            bool carry = false)
            noexcept
        {
            I result;
            bool ovf = __builtin_add_overflow(a, b, &result);
            if (carry)
                ovf |= __builtin_add_overflow(result, I{1}, &result);
            return { result, ovf };
        }
#endif

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




        template<tuple::tuple_like Tup>
        constexpr
        std::pair<Tup, bool>
        add(const Tup& a,
            const Tup& b,
            bool carry_in = false)
            noexcept
        {
            const auto [head_sum, head_ovf] = add(tuple::first(a),
                                                  tuple::first(b),
                                                  carry_in);

            if constexpr (std::tuple_size_v<Tup> > 1) {
                const auto [tail_sum, tail_ovf] = add(tuple::tail(a),
                                                      tuple::tail(b),
                                                      head_ovf);
                return {
                    std::tuple_cat(std::tuple{head_sum}, tail_sum),
                    tail_ovf
                };
            } else
                return { head_sum, head_ovf };
        }


    } // namespace overflow



    template<tuple::tuple_like Tup>
    constexpr
    Tup
    add(const Tup& a,
        const Tup& b,
        bool carry_in = false)
        noexcept
    {
        auto [sum, ovf] = overflow::add(tuple::first(a),
                                        tuple::first(b),
                                        carry_in);
        if constexpr (std::tuple_size_v<Tup> > 1)
            return std::tuple_cat(std::tuple{sum},
                                  add(tuple::tail(a),
                                      tuple::tail(b),
                                      ovf));
        else
            return { sum };
    }


    template<std::integral I>
    constexpr
    I
    add(I a,
        I b,
        bool carry_in = false)
    {
        return a + b + carry_in;
    }



} // namespace fxd::utils::add


#endif
