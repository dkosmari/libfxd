#ifndef LIBFXD_UTILS_SHIFT_HPP
#define LIBFXD_UTILS_SHIFT_HPP

#include <concepts>
#include <limits>

#include "types.hpp"
#include "utils-add.hpp"
#include "utils-tuple.hpp"
#include "utils.hpp"


namespace fxd::utils::shift {


    template<template<typename...> typename Tuple,
             typename... I>
    concept tuple_of_similar_ints = tuple::tuple_like<Tuple<I...>> &&
        (std::integral<I> && ...) &&
        ((sizeof(I) == sizeof(tuple::first_element_t<Tuple<I...>>)) && ...);



    template<std::integral I>
    ALWAYS_INLINE
    constexpr
    I
    make_bias(unsigned bits)
        noexcept
    {
        constexpr unsigned w = type_width<I>;
        if (bits < w)
            return (I{1} << bits) - I{1};
        else
            return ~I{0};
    }


    template<std::integral I>
    ALWAYS_INLINE
    constexpr
    I
    make_bias_for(unsigned bits,
                  I)
        noexcept
    {
        return make_bias<I>(bits);
    }




    template<std::integral... I,
             std::size_t... Idx>
    ALWAYS_INLINE
    constexpr
    std::tuple<I...>
    make_bias_tuple_helper(unsigned bits,
                           std::index_sequence<Idx...>)
        noexcept
    {
        using Tup = std::tuple<I...>;
        using Elem = std::tuple_element_t<0, Tup>;
        constexpr unsigned w = type_width<Elem>;

        const std::size_t partial_idx = bits / w;
        const unsigned partial_off = bits % w;
        const Elem partial_elem = make_bias<Elem>(partial_off);
        return { (
                  Idx > partial_idx
                  ? Elem{0}
                  : (
                     Idx < partial_idx
                     ? ~Elem{0}
                     : partial_elem
                    )
                  ) ... };
    }


    template<std::integral... I>
    ALWAYS_INLINE
    constexpr
    std::tuple<I...>
    make_bias_tuple(unsigned bits)
        noexcept
    {
        using Seq = std::index_sequence_for<I...>;
        return make_bias_tuple_helper<I...>(bits, Seq{});
    }


    template<template<typename...> typename Tup,
             std::integral... I>
    ALWAYS_INLINE
    constexpr
    std::tuple<I...>
    make_bias_for(unsigned bits,
                  const Tup<I...>&)
        noexcept
    {
        return make_bias_tuple<I...>(bits);
    }


    template<std::integral I>
    ALWAYS_INLINE
    constexpr
    I
    shl_real(I a,
             unsigned b)
        noexcept
    {
        if (b >= type_width<I>)
            return 0;
        return a << b;
    }


    template<std::integral I>
    constexpr I shr_real(I a, unsigned b) noexcept;


    template<std::integral I>
    ALWAYS_INLINE
    constexpr
    I
    shl(I a,
        int b)
        noexcept
    {
        if (b < 0)
            return shr_real(a, -b);
        return shl_real(a, b);
    }


    template<std::integral I>
    ALWAYS_INLINE
    constexpr
    I
    shr_real(I a,
             unsigned b)
        noexcept
    {
        if (b >= type_width<I>)
            return a < 0 ? -1 : 0;
        return a >> b;
    }


    template<std::integral I>
    ALWAYS_INLINE
    constexpr
    I
    shr(I a,
        int b)
        noexcept
    {
        if (b < 0)
            return shl_real(a, -b);
        return shr_real(a, b);
    }


    // tuple versions: little-endian


    template<std::size_t At,
             tuple::tuple_like Tup,
             std::size_t... Idx>
    ALWAYS_INLINE
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
    ALWAYS_INLINE
    constexpr
    Tup
    shl_helper(const Tup& a,
               int b,
               std::index_sequence<Idx...> seq)
        noexcept
    {
        return { shl_at<Idx>(a, b, seq) ... };
    }


    template<template<typename...> typename Tuple,
             std::integral... I>
    requires tuple_of_similar_ints<Tuple, I...>
    ALWAYS_INLINE
    constexpr
    Tuple<I...>
    shl_real(const Tuple<I...>& a,
             int b)
        noexcept
    {
        using Seq = std::index_sequence_for<I...>;
        return shl_helper(a, b, Seq{});
    }



    template<template<typename...> typename Tuple, std::integral... I>
    requires tuple_of_similar_ints<Tuple, I...>
    constexpr Tuple<I...> shr_real(const Tuple<I...>& a, int b) noexcept;



    template<template<typename...> typename Tuple,
             std::integral... I>
    requires tuple_of_similar_ints<Tuple, I...>
    ALWAYS_INLINE
    constexpr
    Tuple<I...>
    shl(const Tuple<I...>& a,
        int b)
        noexcept
    {
        if (b < 0)
            return shr_real(a, -b);
        return shl_real(a, b);
    }



    template<std::size_t At,
             tuple::tuple_like Tup,
             std::size_t... Idx>
    ALWAYS_INLINE
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
    ALWAYS_INLINE
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
    requires tuple_of_similar_ints<Tuple, I...>
    ALWAYS_INLINE
    constexpr
    Tuple<I...>
    shr_real(const Tuple<I...>& a,
             int b)
        noexcept
    {
        using Seq = std::index_sequence_for<I...>;
        return shr_helper(a, b, Seq{});
    }


    template<template<typename...> typename Tuple,
             std::integral... I>
    requires tuple_of_similar_ints<Tuple, I...>
    ALWAYS_INLINE
    constexpr
    Tuple<I...>
    shr(const Tuple<I...>& a,
         int b)
        noexcept
    {
        if (b < 0)
            return shl_real(a, -b);
        return shr_real(a, b);
    }



}


#endif
