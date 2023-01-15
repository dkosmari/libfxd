#ifndef LIBFXD_TRAITS_HPP
#define LIBFXD_TRAITS_HPP

#include <concepts>
#include <type_traits>

#include "types.hpp"
#include "fixed.hpp"


namespace fxd {


    template<typename T>
    struct is_fixed_point : std::false_type {};

    template<int Int,
             int Frac,
             typename Raw>
    struct is_fixed_point<fixed<Int, Frac, Raw>> :
        std::true_type {};



    template<typename T>
    struct is_signed_fixed_point : std::false_type {};

    template<int Int,
             int Frac,
             std::signed_integral Raw>
    struct is_signed_fixed_point<fixed<Int, Frac, Raw>> :
        std::true_type {};



    template<typename T>
    struct is_unsigned_fixed_point : std::false_type {};

    template<int Int,
             int Frac,
             std::unsigned_integral Raw>
    struct is_unsigned_fixed_point<fixed<Int, Frac, Raw>> :
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


namespace std {


    template<typename Fxd>
    requires (fxd::is_fixed_point_v<Fxd>)
    struct common_type<Fxd, Fxd> {
        using type = Fxd;
    };


    template<typename Fxd,
             typename Other>
    requires (fxd::is_fixed_point_v<Fxd> && is_arithmetic_v<Other>)
    struct common_type<Fxd, Other> {
        using type = Fxd;
    };


    template<typename Other,
             typename Fxd>
    requires (is_arithmetic_v<Other> && fxd::is_fixed_point_v<Fxd>)
    struct common_type<Other, Fxd> {
        using type = Fxd;
    };



}



#endif
