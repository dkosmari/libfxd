/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_IMPL_TYPES_HPP
#define LIBFXD_IMPL_TYPES_HPP

#include <concepts>
#include <cstdint>
#include <limits>
#include <type_traits>


#if (__SIZEOF_INT128__ == 16)
// __int128 exists
#define LIBFXD_PARTIAL_INT128

#if defined(__GLIBCXX_TYPE_INT_N_0) && (__GLIBCXX_BITSIZE_INT_N_0 == 128)
// With -std=gnu++*, __int128 is fully supported by the standard library.
#define LIBFXD_FULL_INT128
#endif

#endif

// TODO: detect __float128 and include a numeric_limits specialization for it.


namespace fxd::impl {



#ifdef LIBFXD_PARTIAL_INT128

    using int128_t = __int128;
    using uint128_t = unsigned __int128;

#endif


#ifdef LIBFXD_FULL_INT128

    static_assert(std::integral<int128_t>);
    static_assert(std::integral<uint128_t>);

#endif


    // How many bits a given integral type has.
    template<typename T>
    requires (std::numeric_limits<T>::is_specialized)
    constexpr inline
    int type_width = std::numeric_limits<T>::digits +
                     (std::numeric_limits<T>::is_signed ? 1 : 0);


    // Select a signed integer by its bit size.
    template<int>
    struct select_int { using type = void; };

    // Select an unsigned integer by its bit size.
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


#ifdef LIBFXD_FULL_INT128

    template<int bits>
    requires (bits > 64 && bits <= 128)
    struct select_int<bits> { using type = int128_t; };

    template<int bits>
    requires (bits > 64 && bits <= 128)
    struct select_uint<bits> { using type = uint128_t; };

#endif


    template<int bits>
    using select_int_t = typename select_int<bits>::type;


    template<int bits>
    using select_uint_t = typename select_uint<bits>::type;




    // Select int that matches signedness of argument

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



    // Check if specialization is available.

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


#ifdef LIBFXD_FULL_INT128
    using intmax_t = int128_t;
    using uintmax_t = uint128_t;
#else
    using std::intmax_t;
    using std::uintmax_t;
#endif

    template<typename I>
    using max_int_for = std::conditional_t<std::numeric_limits<I>::is_signed,
                                           intmax_t,
                                           uintmax_t>;



    // Select a floating point by the mantissa size.
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


#ifdef LIBFXD_HAVE_FLOAT128
    template<int bits>
    requires(bits > std::numeric_limits<long double>::digits &&
             bits <= std::numeric_limits<float128_t>::digits)
    struct select_float<bits> {
        using type = float128_t;
    };

#endif



    // Helper alias for select_float.
    template<int bits>
    using select_float_t = typename select_float<bits>::type;


}


#endif
