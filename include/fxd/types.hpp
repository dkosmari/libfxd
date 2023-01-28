/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_TYPES_HPP
#define LIBFXD_TYPES_HPP

#include <cstdint>
#include <limits>
#include <type_traits>


namespace fxd {


    template<typename T>
    requires (std::numeric_limits<T>::is_specialized)
    constexpr inline
    int type_width = std::numeric_limits<T>::digits +
                     (std::numeric_limits<T>::is_signed ? 1 : 0);

    constexpr int max_type_width = 64;


    template<int>
    struct select_int { using type = void; };

    template<int>
    struct select_uint { using type = void; };



    template<int bits>
    requires (bits <= 8)
    struct select_int<bits> { using type = std::int8_t; };

    template<int bits>
    requires (bits <= 8)
    struct select_uint<bits> { using type = std::uint8_t; };


    template<int bits>
    requires (bits > 8 && bits <= 16)
    struct select_int<bits> { using type = std::int16_t; };

    template<int bits>
    requires (bits > 8 && bits <= 16)
    struct select_uint<bits> { using type = std::uint16_t; };


    template<int bits>
    requires (bits > 16 && bits <= 32)
    struct select_int<bits> { using type = std::int32_t; };

    template<int bits>
    requires (bits > 16 && bits <= 32)
    struct select_uint<bits> { using type = std::uint32_t; };


    template<int bits>
    requires (bits > 32 && bits <= 64)
    struct select_int<bits> { using type = std::int64_t; };

    template<int bits>
    requires (bits > 32 && bits <= 64)
    struct select_uint<bits> { using type = std::uint64_t; };


    template<int bits>
    using select_int_t = typename select_int<bits>::type;


    template<int bits>
    using select_uint_t = typename select_uint<bits>::type;




    // select int that matches signedness of argument

    template<int bits,
             typename I>
    using select_int_for = std::conditional_t<
        std::numeric_limits<I>::is_signed,
        select_int_t<bits>,
        select_uint_t<bits>
        >;

    template<typename I>
    using double_width_t = select_int_for<2 * type_width<I>, I>;

    template<typename I>
    using half_width_t = select_int_for<type_width<I> / 2, I>;



    // check if specialization is available

    template<int bits>
    constexpr inline
    bool has_int = !std::is_void_v<select_int_t<bits>>;


    template<int bits>
    constexpr inline
    bool has_uint = !std::is_void_v<select_uint_t<bits>>;


    template<int bits,
             typename I>
    constexpr inline
    bool has_int_for = !std::is_void_v<select_int_for<bits, I>>;


    // template<typename I>
    // inline constexpr
    // bool has_wider_v = !std::is_void_v<wider_t<I>>;




    // select floating point type by the mantissa size


    template<int>
    struct select_float {
        using type = void;
    };


    template<int bits>
    requires(bits <= std::numeric_limits<float>::digits)
    struct select_float<bits> {
        using type = float;
    };


    template<int bits>
    requires(bits > std::numeric_limits<float>::digits &&
             bits <= std::numeric_limits<double>::digits)
    struct select_float<bits> {
        using type = double;
    };


    template<int bits>
    requires(bits > std::numeric_limits<double>::digits &&
             bits <= std::numeric_limits<long double>::digits)
    struct select_float<bits>  {
        using type = long double;
    };


    template<int bits>
    using select_float_t = typename select_float<bits>::type;


}


#endif
