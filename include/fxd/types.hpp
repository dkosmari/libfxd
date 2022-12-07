#ifndef LIBFXD_TYPES_HPP
#define LIBFXD_TYPES_HPP

#include <cstdint>
#include <limits>
#include <type_traits>


namespace fxd {



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


#ifdef __SIZEOF_INT128__

    template<int bits>
    requires (bits > 64 && bits <= 128
              && std::numeric_limits<__int128>::is_specialized
              && std::is_integral_v<__int128>)
    struct select_int<bits> { using type = __int128; };

    template<int bits>
    requires (bits > 64 && bits <= 128
              && std::numeric_limits<unsigned __int128>::is_specialized
              && std::is_integral_v<__int128>)
    struct select_uint<bits> { using type = unsigned __int128; };

#endif


    // client is allowed to add wider specializations if they're available


    template<int bits>
    using select_int_t = select_int<bits>::type;


    template<int bits>
    using select_uint_t = select_uint<bits>::type;





    // select integer that has twice as many bits
    template<typename I>
    using wider_t =
        std::conditional_t<std::numeric_limits<I>::is_signed,
                           select_int_t<2 * (std::numeric_limits<I>::digits + 1)>,
                           select_uint_t<2 * std::numeric_limits<I>::digits>>;

    template<typename I>
    inline constexpr
    bool has_wider_v = !std::is_void_v<wider_t<I>>;



}


#endif
