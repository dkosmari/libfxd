/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_IMPL_SHIFT_HPP
#define LIBFXD_IMPL_SHIFT_HPP

#include <concepts>
#include <limits>

#include "types.hpp"
#include "add.hpp"
#include "tuple.hpp"


namespace fxd::impl {


    template<std::integral I>
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
             tuple_like Tup,
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



    template<tuple_like Tup,
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


    template<template<typename...> typename Tuple,
             std::integral... I>
    requires tuple_of_similar_ints<Tuple, I...>
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
             tuple_like Tup,
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



    template<tuple_like Tup,
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
    requires tuple_of_similar_ints<Tuple, I...>
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





    namespace overflow {

        template<typename T>
        constexpr
        std::pair<T, bool>
        shl_real(T a,
                 unsigned b)
            noexcept
        {
            const T result = impl::shl_real(a, b);
            const bool ovf = a != impl::shr_real(result, b);
            return { result, ovf };
        }


        template<typename T>
        constexpr
        std::pair<T, bool>
        shr_real(T a,
                 unsigned b)
            noexcept
        {
            const T result = impl::shr_real(a, b);
            const bool ovf = a != impl::shl_real(a, b);
            return { result, ovf };
        }


        template<typename T>
        std::pair<T, bool>
        shl(T a,
            int b)
        {
            if (b < 0)
                return shr_real(a, -b);
            else
                return shl_real(a, b);
        }


        template<typename T>
        std::pair<T, bool>
        shr(T a,
            int b)
        {
            if (b < 0)
                return shl_real(a, -b);
            else
                return shr_real(a, b);
        }


    } // namespace overflow


} // namespace fxd::impl


#endif
