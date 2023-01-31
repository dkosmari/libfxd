/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_IMPL_BIAS_HPP
#define LIBFXD_IMPL_BIAS_HPP

#include <concepts>
#include <cstddef>
#include <tuple>
#include <type_traits>

#include "../types.hpp"


namespace fxd::impl {


    template<std::integral I>
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
    constexpr
    std::tuple<I...>
    make_bias_for(unsigned bits,
                  const Tup<I...>&)
        noexcept
    {
        return make_bias_tuple<I...>(bits);
    }


} // namespace fxd::impl


#endif
