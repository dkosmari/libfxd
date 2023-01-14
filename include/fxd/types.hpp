#ifndef LIBFXD_TYPES_HPP
#define LIBFXD_TYPES_HPP

#include <cstdint>
#include <limits>
#include <type_traits>


namespace fxd {

    
    template<typename T>
    requires(std::numeric_limits<T>::is_specialized)
    constexpr inline
    int type_width = std::numeric_limits<T>::digits +
                     (std::numeric_limits<T>::is_signed ? 1 : 0);


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


    // client is allowed to add wider specializations if they're available


    template<int bits>
    using select_int_t = typename select_int<bits>::type;


    template<int bits>
    using select_uint_t = typename select_uint<bits>::type;





    // select integer that has twice as many bits
    
    template<typename I>
    using wider_t = std::conditional_t<std::numeric_limits<I>::is_signed,
                                       select_int_t<2 * type_width<I>>,
                                       select_uint_t<2 * type_width<I>>>;

    template<typename I>
    inline constexpr
    bool has_wider_v = !std::is_void_v<wider_t<I>>;


    
    // select integer that has half as many bits
    
    template<typename I>
    using narrower_t = std::conditional_t<std::numeric_limits<I>::is_signed,
                                          select_int_t<type_width<I> / 2>,
                                          select_uint_t<type_width<I> / 2>>;

    // template<typename I>
    // inline constexpr
    // bool has_narrower_v = !std::is_void_v<narrower_t<I>>; 


    

    // select floating point type by the mantissa bits


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
