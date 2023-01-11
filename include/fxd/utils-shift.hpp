#ifndef LIBFXD_UTILS_SHIFT_HPP
#define LIBFXD_UTILS_SHIFT_HPP

#include <concepts>
#include <limits>
#include <utility>

#include "types.hpp"
#include "utils-tuple.hpp"
#include "utils-add.hpp"


namespace fxd::utils::shift {


    template<std::integral I>
    constexpr I shr(I a, int b) noexcept;


    // same as '<<' but safely handles any shift offset
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


    // same as '>>' but safely handles any shift offset
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
    constexpr I shrz(I a, int b) noexcept;


    // shift left rounding to zero
    // same as multiplying or dividing by 2^b
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


    // shift right rounding to zero
    // same as dividing or multiplying by 2^b
    template<std::integral I>
    constexpr
    I
    shrz(I a,
         int b)
        noexcept
    {
        if constexpr (!std::numeric_limits<I>::is_signed)
            return shr(a, b);

        if (a >= 0)
            return shr(a, b);

        if (b < 0)
            return shl(a, -b);

        if (b > std::numeric_limits<I>::digits)
            return 0;

        if (b == std::numeric_limits<I>::digits)
            // avoid dividing MIN / MIN
            return a == std::numeric_limits<I>::min() ? -1 : 0;

        // d will never be negative
        const I d = I{1} << b;
        return a / d;
    }





    // tuple versions: little-endian


    template<std::size_t At,
             tuple::tuple_like Tup,
             std::size_t... Idx>
    constexpr
    std::tuple_element_t<At, Tup>
    shl_at(const Tup& a,
           int b,
           std::index_sequence<Idx...>)
        noexcept
    {
        using Elem = std::tuple_element_t<At, Tup>;
        constexpr int w = type_width<Elem>;
        return (shl(get<Idx>(a),
                    b + (w * (static_cast<int>(Idx) - static_cast<int>(At))))
                | ...);
    }



    template<tuple::tuple_like Tup,
             std::size_t... Idx>
    constexpr
    Tup
    shl_helper(const Tup& a,
               int b,
               std::index_sequence<Idx...> seq)
        noexcept
    {
        return { shl_at<Idx>(a, b, seq) ... };
    }


    // forward declaration for shl()
    template<template<typename...> typename Tuple, std::integral... I>
    requires utils::tuple::tuple_like<Tuple<I...>>
    constexpr Tuple<I...> shr(const Tuple<I...>& a, int b) noexcept;


    template<template<typename...> typename Tuple,
             std::integral... I>
    requires utils::tuple::tuple_like<Tuple<I...>>
    constexpr
    Tuple<I...>
    shl(const Tuple<I...>& a,
        int b)
        noexcept
    {
        static_assert((std::integral<I> && ...),
                      "all elements must have integral types");
        static_assert(((sizeof(I) == sizeof(tuple::first_element_t<Tuple<I...>>)) && ...),
                      "all elements must have the same size");

        if (b < 0)
            return shr(a, -b);

        using Seq = std::index_sequence_for<I...>;
        return shl_helper(a, b, Seq{});
    }


    // forward declaration for shlz()
    template<template<typename...> typename Tuple, std::integral... I>
    requires utils::tuple::tuple_like<Tuple<I...>>
    constexpr Tuple<I...> shrz(const Tuple<I...>& a, int b) noexcept;



    template<template<typename...> typename Tuple,
             std::integral... I>
    requires utils::tuple::tuple_like<Tuple<I...>>
    constexpr
    Tuple<I...>
    shlz(const Tuple<I...>& a,
         int b)
        noexcept
    {
        static_assert((std::integral<I> && ...),
                      "all elements must have integral types");
        static_assert(((sizeof(I) == sizeof(tuple::first_element_t<Tuple<I...>>)) && ...),
                      "all elements must have the same size");

        if (b < 0)
            return shrz(a, -b);

        return shl(a, b);
    }




    template<std::size_t At,
             tuple::tuple_like Tup,
             std::size_t... Idx>
    constexpr
    std::tuple_element_t<At, Tup>
    shr_at(const Tup& a,
           int b,
           std::index_sequence<Idx...>)
        noexcept
    {
        using Elem = std::tuple_element_t<At, Tup>;
        constexpr int w = type_width<Elem>;
        return (shr(get<Idx>(a),
                    b + (w * (int(At) - int(Idx))))
                | ...);
    }



    template<tuple::tuple_like Tup,
             std::size_t... Idx>
    constexpr
    Tup
    shr_helper(const Tup& a,
               int b,
               std::index_sequence<Idx...> seq)
        noexcept
    {
        return {
            shr_at<Idx>(a, b, seq) ...
        };
    }



    template<template<typename...> typename Tuple,
             std::integral... I>
    requires utils::tuple::tuple_like<Tuple<I...>>
    constexpr
    Tuple<I...>
    shr(const Tuple<I...>& a,
         int b)
        noexcept
    {
        static_assert((std::integral<I> && ...),
                      "all elements must have integral types");
        static_assert(((sizeof(I) == sizeof(tuple::first_element_t<Tuple<I...>>)) && ...),
                      "all elements must have the same size");

        if (b < 0)
            return shl(a, -b);

        using Seq = std::index_sequence_for<I...>;
        return shr_helper(a, b, Seq{});
    }


    template<std::integral... I,
             std::size_t... Idx>
    constexpr
    std::tuple<I...>
    make_bias_helper(int bits,
                     std::index_sequence<Idx...>)
        noexcept
    {
        using Tup = std::tuple<I...>;
        using Elem = std::tuple_element_t<0, Tup>;
        constexpr int w = type_width<Elem>;

        const std::size_t partial_idx = bits / w;
        const int partial_off = bits - w * static_cast<int>(partial_idx);
        return { (
                  Idx > partial_idx
                  ? Elem{0}
                  : (
                     Idx < partial_idx
                     ? ~Elem{0}
                     : (Elem{1} << partial_off) - Elem{1}
                    )
                  ) ... };
    }


    template<std::integral... I>
    constexpr
    std::tuple<I...>
    make_bias(int bits)
        noexcept
    {
        using Seq = std::index_sequence_for<I...>;
        return make_bias_helper<I...>(bits, Seq{});
    }


    template<template<typename...> typename Tuple,
             std::integral... I>
    requires utils::tuple::tuple_like<Tuple<I...>>
    constexpr
    Tuple<I...>
    shrz(const Tuple<I...>& a,
         int b)
        noexcept
    {
        static_assert((std::integral<I> && ...),
                      "all elements must have integral types");
        static_assert(((sizeof(I) == sizeof(tuple::first_element_t<Tuple<I...>>)) && ...),
                      "all elements must have the same size");

        if constexpr ((std::unsigned_integral<I> && ...))
            return shr(a, b);

        if (b < 0)
            return shlz(a, -b);

        if (tuple::last(a) < 0) {
            // We must add a bias to ensure it's rounded correctly before the shift
            const auto bias = make_bias<I...>(b);
            const auto biased_a = add::add(std::tuple{a}, bias);
            return std::make_from_tuple<Tuple<I...>>(shr(biased_a, b));
        } else
              return shr(a, b);
    }


}


#endif
