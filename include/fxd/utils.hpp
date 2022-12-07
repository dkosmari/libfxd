#ifndef LIBFXD_UTILS_HPP
#define LIBFXD_UTILS_HPP


#include <algorithm>
#include <bit>
#include <concepts>
#include <limits>
#include <type_traits>
#include <utility>


namespace fxd::utils {


    template<typename T>
    requires(std::numeric_limits<T>::is_specialized)
    constexpr inline
    int type_width = std::numeric_limits<T>::digits +
        (std::numeric_limits<T>::is_signed ? 1 : 0);



    template<std::integral I>
    constexpr I shr(I a, int b) noexcept;


    template<std::integral I>
    constexpr I shrz(I a, int b) noexcept;



    template<std::unsigned_integral U>
    constexpr
    std::pair<U, U>
    add_pair(const std::pair<U, U>& a,
             const std::pair<U, U>& b)
        noexcept;



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
    shl(const std::pair<U, I>& a,
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
            return shl(a, -b);

        constexpr int w = type_width<I>;

        if (a.second < 0) {

            U bias0 = shl<U>(1, b) - 1;
            I bias1 = shl<I>(1, b - w) - (b >= w ? 1 : 0);

            auto [c0, c1] = add_pair<U>(a, {bias0, bias1});

            return {
                shr(c0, b) | shl(c1, w - b),
                shr<U>(c1, b)
            };

        } else {

            return {
                shr(a.first, b) | shl(a.second, w - b),
                shr(a.second, b)
            };

        }

    }




    template<std::unsigned_integral U>
    constexpr
    std::pair<U, U>
    add_pair(const std::pair<U, U>& a,
             const std::pair<U, U>& b)
        noexcept
    {
        U c0;
        U c1 = __builtin_add_overflow(a.first, b.first, &c0)
            + a.second + b.second;
        return {c0, c1};
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

        const I a0 = a & mask;
        const I a1 = a >> k; // note: may sign-extend

        const I b0 = b & mask;
        const I b1 = b >> k; // note: may sign-extend

        const I a0b1 = a0 * b1;
        const I a1b0 = a1 * b0;

        U c01 = static_cast<U>(a0) * static_cast<U>(b0);
        U c23 = a1 * b1 + (a0b1 >> k) + (a1b0 >> k)
            + __builtin_add_overflow(c01,
                                     static_cast<U>(a0b1 & mask) << k,
                                     &c01)
            + __builtin_add_overflow(c01,
                                     static_cast<U>(a1b0 & mask) << k,
                                     &c01);

        return {c01, c23};
    }



    template<std::unsigned_integral U>
    std::pair<U, U>
    full_div(U a,
             U b,
             int frac_bits)
        noexcept
    {
        constexpr int k = type_width<U>;
        constexpr U top_bit = U{1} << (k - 1);
        U q0 = 0;
        const U q1 = a / b;
        U r = a % b;
        for (int i = 0; r && i < std::min(k, frac_bits); ++i) {

            // this would be the carry out of the shift below
            bool carry = r & top_bit;
            r <<= 1;

            if (carry || r >= b) {
                r -= b;
                q0 += U{1} << (k - i - 1);
            }
        }
        return {q0, q1};
    }


}


#endif
