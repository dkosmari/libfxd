#ifndef LIBFXD_UTILS_HPP
#define LIBFXD_UTILS_HPP


#include <algorithm>
#include <bit>
#include <cfenv>
#include <concepts>
#include <limits>
#include <type_traits>
#include <utility>
#include <tuple>

#if 0
#include <iostream>
#include <iomanip>
#endif

#ifdef _MSC_VER
#include <intrin.h>
#endif


#if defined(__GNUC__)                                                   \
    || defined(__clang__)                                               \
    || defined(__INTEL_COMPILER) || defined(__INTEL_LLVM_COMPILER)
// every non-MS compiler is compatible with GCC's builtins
#define LIBFXD_GCC_BUILTINS
#endif

#if defined(_MSC_VER)
#define LIBFXD_INTEL_BUILTINS
#endif


namespace fxd::utils {


    template<typename T>
    requires(std::numeric_limits<T>::is_specialized)
    constexpr inline
    int type_width = std::numeric_limits<T>::digits +
        (std::numeric_limits<T>::is_signed ? 1 : 0);



#if 0
    template<typename I>
    void
    show(const char* name, I val)
    {
        constexpr int k = type_width<I>;
        using U = std::make_unsigned_t<I>;

        std::cout << std::setw(6) << name << " = ";

        std::cout << std::hex << std::setfill('0');

        std::cout <<  std::setw(k / 4)
                  << static_cast<U>(val);

        std::cout << std::dec
                  << std::setfill(' ')
                  << "  = " << val
                  << std::endl;
    }

#define DUMP(x) \
        show(#x, x)
#endif



    template<typename Head,
             typename... Tail>
    constexpr
    Head
    head(const std::tuple<Head, Tail...>& t)
        noexcept
    {
        return get<0>(t);
    }


    template<typename Head,
             typename... Tail,
             std::size_t... Idx>
    constexpr
    std::tuple<Tail...>
    tail_helper(const std::tuple<Head, Tail...>& t,
                std::index_sequence<Idx...>)
        noexcept
    {
        return { get<1 + Idx>(t) ... };
    }


    template<typename Head,
             typename... Tail>
    constexpr
    std::tuple<Tail...>
    tail(const std::tuple<Head, Tail...>& t)
        noexcept
    {
        using Idx = std::index_sequence_for<Tail...>;
        return tail_helper(t, Idx{});
    }



    template<std::unsigned_integral U>
    constexpr
    std::pair<U, bool>
    add_overflow(U a,
                 U b,
                 bool carry = false)
        noexcept
    {
#if defined(LIBFXD_GCC_BUILTINS)
        U ab;
        U abc;
        const bool o1 = __builtin_add_overflow(a, b, &ab);
        const bool o2 = __builtin_add_overflow(ab, carry, &abc);
        const bool overflow = o1 || o2;
#elif defined (LIBFXD_INTEL_BUILTINS)
        U abc;
        bool overflow;
        if constexpr (sizeof(U) == 1)
            overflow = _addcarry_u8(carry, a, b, &abc);
        else if constexpr (sizeof(U) == 2)
            overflow = _addcarry_u16(carry, a, b, &abc);
        else if constexpr (sizeof(U) == 4)
            overflow = _addcarry_u32(carry, a, b, &abc);
        else if constexpr (sizeof(U) == 8)
            overflow = _addcarry_u64(carry, a, b, &abc);
        else
            static_assert(false, "No Intel intrinsic for this integer size.");
#else
#warning No builtin intrinsics to implement add_overflow(), this method is very slow.
        const U ab = a + b;
        const U abc = a + b + carry;
        constexpr U max = std::numeric_limits<U>::max();
        const bool overflow = (a > max - b) || (ab > max - carry);
#endif
        return {abc, overflow};
    }


    template<std::signed_integral I>
    constexpr
    std::pair<I, bool>
    add_overflow(I a,
                 I b,
                 bool carry = false)
        noexcept
    {
#if defined(LIBFXD_GCC_BUILTINS)
        I ab;
        I abc;
        const bool o1 = __builtin_add_overflow(a, b, &ab);
        const bool o2 = __builtin_add_overflow(ab, carry, &abc);
        const bool overflow = o1 || o2;
#elif defined (LIBFXD_INTEL_BUILTINS)
#error Not implemented yet.
#else
#warning Not efficient.
        constexpr I max = std::numeric_limits<I>::max();
        constexpr I min = std::numeric_limits<I>::min();
        using U = std::make_unsigned_t<I>;
        const I ab  = static_cast<U>(a) + static_cast<U>(b);
        const I abc = static_cast<U>(a) + static_cast<U>(b) + static_cast<U>(carry);
        const bool overflow =
            (b > 0 && a > max - b) // a + b > max
            ||
            (b < 0 && a < min - b) // a + b < min
            ||
            (c && ab > max - c); // a + b + c > max
#endif
        return {abc, overflow};
    }


    template<std::integral I>
    constexpr
    std::pair<I, bool>
    sub_overflow(I a,
                 I b,
                 bool borrow = false)
    {
#if defined(LIBFXD_GCC_BUILTINS)
        I ab;
        I abb;
        const bool o1 = __builtin_sub_overflow(a, b, &ab);
        const bool o2 = __builtin_sub_overflow(ab, borrow, &abb);
        const bool overflow = o1 || o2;
#else
#error Not implemented yet.
#endif
        return {abb, overflow};
    }



    template<std::integral I>
    constexpr
    std::tuple<I>
    add(const std::tuple<I>& a,
        const std::tuple<I>& b,
        bool carry_in = false)
        noexcept
    {
        auto [sum, overflow] = add_overflow(get<0>(a), get<0>(b), carry_in);
        return std::tuple{sum};
    }


    template<std::integral Head,
             std::integral NextHead,
             std::integral... Tail>
    constexpr
    std::tuple<Head, NextHead, Tail...>
    add(const std::tuple<Head, NextHead, Tail...>& a,
        const std::tuple<Head, NextHead, Tail...>& b,
        bool carry_in = false)
        noexcept
    {
        auto [sum, overflow] = add_overflow(head(a), head(b), carry_in);
        return std::tuple_cat(std::tuple{sum},
                              add(tail(a), tail(b), overflow));
    }


    template<std::integral X,
             std::integral Y>
    constexpr
    std::tuple<X, Y>
    add(const std::pair<X, Y>& a,
        const std::pair<X, Y>& b,
        bool carry_in = false)
    {
        return add(std::tuple{a}, std::tuple{b}, carry_in);
    }



    template<std::integral I>
    constexpr I shr(I a, int b) noexcept;


    template<std::integral I>
    constexpr I shrz(I a, int b) noexcept;


    template<std::integral I>
    constexpr
    I
    shl(I a,
        int b)
        noexcept
    {
        if (b < 0)
            return shr(a, -b);

        if (b >= type_width<I>)
            return 0;

        return a << b;
    }



    template<std::integral I>
    constexpr
    I
    shr(I a,
        int b)
        noexcept
    {
        if (b < 0)
            return shl(a, -b);

        if (b >= type_width<I>)
            return a < 0 ? -1 : 0;

        return a >> b;
    }


    template<std::integral I>
    constexpr
    I
    shlz(I a,
         int b)
        noexcept
    {
        if (b < 0)
            return shrz(a, -b);

        if (b >= type_width<I>)
            return 0;

        return a << b;
    }


    // note: this will round negative integers towards zero, so not really a shift
    // but a division by 2^b
    template<std::integral I>
    constexpr
    I
    shrz(I a,
         int b)
        noexcept
    {
        if constexpr (!std::numeric_limits<I>::is_signed)
            return shr(a, b);

        if (b < 0)
            return shlz(a, -b);

        if (a >= 0)
            return shr(a, b);

        if (b > std::numeric_limits<I>::digits)
            return 0;

        if (b == std::numeric_limits<I>::digits)
            // avoid dividing INT_MIN / INT_MIN
            return a == std::numeric_limits<I>::lowest() ? -1 : 0;


        // d will never be negative
        const I d = I{1} << b;
        return a / d;
    }


    template<std::unsigned_integral U,
             std::integral I>
    constexpr
    std::pair<U, I>
    shrz(const std::pair<U, I>& a,
         int b) noexcept;


    template<std::unsigned_integral U,
             std::integral I>
    constexpr
    std::pair<U, I>
    shlz(const std::pair<U, I>& a,
         int b)
        noexcept
    {
        static_assert(sizeof(U) == sizeof(I));

        if (b < 0)
            return shrz(a, -b);

        constexpr int w = type_width<I>;

        if (b >= 2*w)
            return {0, 0};

        if (b >= w)
            return {
                0,
                a.first << (b - w)
            };

        return {
            a.first << b,
            (a.second << b) | (a.first >> (w - b))
        };
    }


    template<std::unsigned_integral U,
             std::integral I>
    constexpr
    std::pair<U, I>
    shrz(const std::pair<U, I>& a,
         int b)
        noexcept
    {
        static_assert(sizeof(U) == sizeof(I));

        if (b < 0)
            return shlz(a, -b);

        constexpr int w = type_width<I>;

        if (a.second < 0) {
            U bias0 = shl<U>(1, b) - 1;
            I bias1 = shl<I>(1, b - w) - (b >= w ? 1 : 0);

            auto [c0, c1] = add(a, {bias0, bias1});

            return {
                shr(c0, b) | shl(c1, w - b),
                shr(c1, b)
            };

        } else {

            return {
                shr(a.first, b) | shl(a.second, w - b),
                shr(a.second, b)
            };

        }

    }






    // returns low, high parts of the full multiplication a*b
    // without using larger arithmetic than I
    template<std::integral I>
    constexpr
    std::pair<std::make_unsigned_t<I>, I>
    full_mult(I a,
              I b)
        noexcept
    {
        constexpr int k = type_width<I> / 2;
        using U = std::make_unsigned_t<I>;
        constexpr U mask = (U{1} << k) - 1;

        // DUMP(mask);

        const U a0 = a & mask;
        // DUMP(a0);
        const U a1 = a >> k; // note: may sign-extend before becoming unsigned
        // DUMP(a1);

        const U b0 = b & mask;
        // DUMP(b0);
        const U b1 = b >> k; // note: may sign-extend before becoming unsigned
        // DUMP(b1);

        const I a0b1 = a0 * b1;
        // DUMP(a0b1);
        const I a1b0 = a1 * b0;
        // DUMP(a1b0);

        U c01 = a0 * b0;
        // DUMP(c01);

        U carry1, carry2;
        std::tie(c01, carry1) = add_overflow(c01, static_cast<U>(a0b1 & mask) << k);
        std::tie(c01, carry2) = add_overflow(c01, static_cast<U>(a1b0 & mask) << k);

        I c23 = a1 * b1
            + (a0b1 >> k)
            + (a1b0 >> k)
            + carry1 + carry2;

        // DUMP(c01);
        // DUMP(c23);
        return {c01, c23};
    }


    // TODO: make this return a 3-array or triple
    template<int frac_bits,
             std::unsigned_integral U>
    std::pair<U, U>
    full_div(U a,
             U b)
        noexcept
    {
        constexpr int k = type_width<U>;
        constexpr U top_bit = U{1} << (k - 1);
        U q0 = 0;
        U q1 = a / b;
        U r = a % b;
        for (int i = 0; r && i < std::min(k, frac_bits); ++i) {

            // this would be the carry out of the shift below
            bool carry = r & top_bit;
            r <<= 1;

            if (carry || r >= b) {
                r -= b;
                q0 |= U{1} << (k - i - 1);
            }
        }


        if constexpr (frac_bits > k) {
            // calculate even more bits and shift both q0 and q1 up
            // to accomodate them
            const int extra = frac_bits - k;
            U q00 = 0;
            for (int i = 0; i < extra; ++i) {
                bool carry = r & top_bit;
                r <<= 1;

                if (carry || r >= b) {
                    r -= b;
                    q00 |= U{1} << (k - i - 1);
                }
            }
            q1 = (q1 << extra) | (q0 >> (k - extra));
            q0 = (q0 << extra) | (q00 >> (k - extra));
        }

        return {q0, q1};
    }


#if (defined(__clang__) && __clang_major__ >= 12 && !defined(__FAST_MATH__))
#pragma STDC FENV_ACCESS ON
#endif


    struct rounder {

        int mode;

        rounder()
        noexcept :
            mode{std::fegetround()}
        {
            std::fesetround(FE_TOWARDZERO);
        }

        ~rounder()
            noexcept
        {
            if (mode >= 0)
                std::fesetround(mode);
        }

    };

}


#undef DUMP


#endif
