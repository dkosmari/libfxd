#ifndef LIBFXD_TRAITS_HPP
#define LIBFXD_TRAITS_HPP

#include <algorithm>
#include <cmath>
#include <limits>
#include <type_traits>

#include "types.hpp"
#include "fixed.hpp"


namespace fxd {


    template<typename T>
    struct is_fixed_point : std::false_type {};

    template<int Int,
             int Frac,
             typename T>
    struct is_fixed_point<fixed<Int, Frac, T>> :
        std::true_type {};



    template<typename T>
    struct is_signed_fixed_point : std::false_type {};

    template<int Int,
             int Frac,
             typename I>
    requires(std::numeric_limits<I>::is_specialized && !std::numeric_limits<I>::is_signed)
    struct is_signed_fixed_point<fixed<Int, Frac, I>> :
        std::true_type {};



    template<typename T>
    struct is_unsigned_fixed_point : std::false_type {};

    template<int Int,
             int Frac,
             typename U>
    requires(std::numeric_limits<U>::is_specialized && !std::numeric_limits<U>::is_signed)
    struct is_unsigned_fixed_point<fixed<Int, Frac, U>> :
        std::true_type {};



    template<typename T>
    inline constexpr
    bool is_fixed_point_v          = is_fixed_point<T>::value;

    template<typename T>
    inline constexpr
    bool is_signed_fixed_point_v   = is_signed_fixed_point<T>::value;

    template<typename T>
    inline constexpr
    bool is_unsigned_fixed_point_v = is_unsigned_fixed_point<T>::value;



}



#endif
