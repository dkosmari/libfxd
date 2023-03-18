/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_DETAIL_ADD_HPP
#define LIBFXD_DETAIL_ADD_HPP

#include <concepts>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

#include "tuple.hpp"


namespace fxd::detail {


    namespace overflow {

#ifdef __amd64__
#define LIBFXD_HAVE_ADD_OVERFLOW_ASM

        template<std::unsigned_integral U>
        requires (type_width<U> <= 64)
        std::pair<U, bool>
        add_asm(U a,
                U b,
                bool carry)
            noexcept
        {
            asm("\n\
        addb    $-1, %[CARRY] \n\
        adc     %[B], %[A] \n\
        setc    %[CARRY] \n\
        "
                : // outputs
                  [A] "+R" (a),
                  [CARRY] "+&q" (carry)
                : // inputs
                  [B] "rm" (b)
                : // clobbers
                  "cc"
                );

            return { a, carry };
        }



        template<std::signed_integral S>
        requires (type_width<S> <= 64)
        std::pair<S, bool>
        add_asm(S a,
                S b,
                bool carry)
            noexcept
        {
            asm("\n\
        addb    $-1, %[CARRY] \n\
        adc     %[B], %[A] \n\
        seto    %[CARRY] \n\
        "
                : // outputs
                  [A] "+R" (a),
                  [CARRY] "+&q" (carry)
                : // inputs
                  [B] "rm" (b)
                : // clobbers
                  "cc"
                );

            return { a, carry };
        }


#endif // __amd64__


#if !defined(LIBFXD_HAVE_ADD_OVERFLOW)                                          \
    && (defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER))
#define LIBFXD_HAVE_ADD_OVERFLOW

        template<std::integral I>
        constexpr
        std::pair<I, bool>
        add(I a,
            I b,
            bool carry)
            noexcept
        {
#ifdef LIBFXD_HAVE_ADD_OVERFLOW_ASM
            if (!std::is_constant_evaluated())
                if constexpr (requires { add_asm<I>(a, b, carry); })
                    return add_asm(a, b, carry);
#endif
            I r1;
            const bool o1 = __builtin_add_overflow(a, b, &r1);
            I r2;
            const bool o2 = __builtin_add_overflow(r1, I{carry}, &r2);
            return { r2, o1 || o2 };
        }


        template<std::integral I>
        constexpr
        std::pair<I, bool>
        add(I a,
            I b)
            noexcept
        {
            I result;
            const bool ovf = __builtin_add_overflow(a, b, &result);
            return { result, ovf };
        }

#endif



#ifndef LIBFXD_HAVE_ADD_OVERFLOW
#define LIBFXD_HAVE_ADD_OVERFLOW

        // fallback implementation using only standard language constructs

        template<std::unsigned_integral U>
        requires (has_uint<type_width<U> + 1>)
        constexpr inline
        std::pair<U, bool>
        add(U a,
            U b,
            bool carry)
            noexcept
        {
            using UU = select_uint_t<type_width<U> + 1>;
            const UU aa = a;
            const UU bb = b;
            const UU result = aa + bb + carry;
            constexpr UU max = std::numeric_limits<U>::max();
            const bool ovf = result > max;
            return { result, ovf };
        }


        template<std::unsigned_integral U>
        requires (has_uint<type_width<U> + 1>)
        constexpr inline
        std::pair<U, bool>
        add(U a,
            U b)
            noexcept
        {
            using UU = select_uint_t<type_width<U> + 1>;
            const UU aa = a;
            const UU bb = b;
            const UU result = aa + bb;
            constexpr UU max = std::numeric_limits<U>::max();
            const bool ovf = result > max;
            return { result, ovf };
        }



        constexpr inline
        std::pair<detail::uintmax_t, bool>
        add(detail::uintmax_t a,
            detail::uintmax_t b,
            bool carry)
            noexcept
        {
            using U = detail::uintmax_t;
            const U ab = a + b;
            const U result = a + b + carry;
            constexpr U max = std::numeric_limits<U>::max();
            const bool ovf = (a > max - b) || (ab > max - carry);
            return { result, ovf };
        }


        constexpr inline
        std::pair<detail::uintmax_t, bool>
        add(detail::uintmax_t a,
            detail::uintmax_t b)
            noexcept
        {
            using U = detail::uintmax_t;
            const U result = a + b;
            constexpr U max = std::numeric_limits<U>::max();
            const bool ovf = (a > max - b);
            return { result, ovf };
        }


        // simply use a larger integer
        template<std::signed_integral S>
        requires (has_int<type_width<S> + 1>)
        constexpr inline
        std::pair<S, bool>
        add(S a,
            S b,
            bool carry)
            noexcept
        {
            using SS = select_int_t<type_width<S> + 1>;
            const SS aa = a;
            const SS bb = b;
            const SS result = aa + bb + carry;
            constexpr SS max = std::numeric_limits<S>::max();
            constexpr SS min = std::numeric_limits<S>::min();
            const bool ovf = result < min || result > max;
            return { result, ovf };
        }


        // simply use a larger integer
        template<std::signed_integral S>
        requires (has_int<type_width<S> + 1>)
        constexpr inline
        std::pair<S, bool>
        add(S a,
            S b)
            noexcept
        {
            using SS = select_int_t<type_width<S> + 1>;
            const SS aa = a;
            const SS bb = b;
            const SS result = aa + bb;
            constexpr SS max = std::numeric_limits<S>::max();
            constexpr SS min = std::numeric_limits<S>::min();
            const bool ovf = result < min || result > max;
            return { result, ovf };
        }


        constexpr inline
        std::pair<detail::intmax_t, bool>
        add(detail::intmax_t a,
            detail::intmax_t b,
            bool carry)
            noexcept
        {
            using S = detail::intmax_t;
            using U = std::make_unsigned_t<S>;
            constexpr S max = std::numeric_limits<S>::max();
            constexpr S min = std::numeric_limits<S>::min();

            const S ab     = static_cast<U>(a) + static_cast<U>(b);
            const S result = static_cast<U>(a) + static_cast<U>(b) + carry;
            const bool ovf =
                (b > 0 && a > max - b) // a + b > max
                ||
                (b < 0 && a < min - b) // a + b < min
                ||
                (carry && ab > max - carry); // a + b + c > max
            return { result, ovf };
        }


        constexpr inline
        std::pair<detail::intmax_t, bool>
        add(detail::intmax_t a,
            detail::intmax_t b)
            noexcept
        {
            using S = detail::intmax_t;
            using U = std::make_unsigned_t<S>;
            constexpr S max = std::numeric_limits<S>::max();
            constexpr S min = std::numeric_limits<S>::min();
            const S result = static_cast<U>(a) + static_cast<U>(b);
            const bool ovf =
                (b > 0 && a > max - b) // a + b > max
                ||
                (b < 0 && a < min - b); // a + b < min
            return { result, ovf };
        }

#endif


        template<tuple_like Tup>
        constexpr
        std::pair<Tup, bool>
        add(const Tup& a,
            const Tup& b,
            bool carry_in = false)
            noexcept
        {
            const auto [head_sum, head_ovf] = add(detail::first(a),
                                                  detail::first(b),
                                                  carry_in);

            if constexpr (std::tuple_size_v<Tup> > 1) {
                const auto [tail_sum, tail_ovf] = add(detail::tail(a),
                                                      detail::tail(b),
                                                      head_ovf);
                return {
                    std::tuple_cat(std::tuple{head_sum}, tail_sum),
                    tail_ovf
                };
            } else
                return { head_sum, head_ovf };
        }


    } // namespace overflow



    template<tuple_like Tup>
    constexpr
    Tup
    add(const Tup& a,
        const Tup& b,
        bool carry_in = false)
        noexcept
    {
        auto [sum, ovf] = overflow::add(detail::first(a),
                                        detail::first(b),
                                        carry_in);
        if constexpr (std::tuple_size_v<Tup> > 1)
            return std::tuple_cat(std::tuple{sum},
                                  add(detail::tail(a),
                                      detail::tail(b),
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
        noexcept
    {
        return a + b + carry_in;
    }



} // namespace fxd::detail


#endif
