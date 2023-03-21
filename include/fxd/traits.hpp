/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_TRAITS_HPP
#define LIBFXD_TRAITS_HPP

#include <concepts>
#include <type_traits>

#include "fixed.hpp"


namespace fxd {

    /// @cond
    template<typename T>
    struct is_fixed_point :
        std::false_type
    {};
    /// @endcond

    /// Test if a type is `fxd::fixed`.
    template<int Int, int Frac, typename Raw>
    struct is_fixed_point<fixed<Int, Frac, Raw>> :
        std::true_type
    {};

    /// Helper template variable for `fxd::is_fixed_point`
    template<typename T>
    inline constexpr
    bool is_fixed_point_v = is_fixed_point<T>::value;


    /// @cond
    template<typename T>
    struct is_signed_fixed_point : std::false_type {};
    /// @endcond

    /// Test if a type is signed `fxd::fixed`.
    template<int Int, int Frac, typename Raw>
    struct is_signed_fixed_point<fixed<Int, Frac, Raw>> :
        std::is_signed<Raw> {};

    /// Helper template variable for `fxd::is_signed_fixed_point`
    template<typename T>
    inline constexpr
    bool is_signed_fixed_point_v = is_signed_fixed_point<T>::value;


    /// @cond
    template<typename T>
    struct is_unsigned_fixed_point : std::false_type {};
    /// @encond

    /// Test if a type is unsigned `fxd::fixed`.
    template<int Int, int Frac, typename Raw>
    struct is_unsigned_fixed_point<fixed<Int, Frac, Raw>> :
        std::is_unsigned<Raw> {};

    /// Helper template variable for `fxd::is_unsigned_fixed_point`
    template<typename T>
    inline constexpr
    bool is_unsigned_fixed_point_v = is_unsigned_fixed_point<T>::value;


    /// Obtain the unsigned version of a `fxd::fixed`.
    template<typename T>
    requires (is_fixed_point_v<T>)
    struct make_unsigned {
        using type = fixed<T::int_bits,
                           T::frac_bits,
                           std::make_unsigned_t<typename T::raw_type>>;
    };

    /// Helper alias for `fxd::make_unsigned`.
    template<typename T>
    using make_unsigned_t = typename make_unsigned<T>::type;


}


namespace std {

    /// Specialization: `fxd::fixed` + `fxd::fixed` = `fxd::fixed`
    template<typename Fxd>
    requires (fxd::is_fixed_point_v<Fxd>)
    struct common_type<Fxd, Fxd> {
        /// @cond
        using type = Fxd;
        /// @endcond
    };


    /// Specialization: both are `fxd::fixed`, and either can be safely converted to the other.
    template<typename Fxd1,
             typename Fxd2>
    requires (fxd::is_fixed_point_v<Fxd1> && fxd::is_fixed_point_v<Fxd2>
              &&
              (is_convertible_v<Fxd1, Fxd2> || is_convertible_v<Fxd2, Fxd1>))
    struct common_type<Fxd1, Fxd2> {
        using type = conditional_t<
            is_convertible_v<Fxd2, Fxd1>,
            Fxd1,
            Fxd2
            >;
    };


    /// Specialization: `fxd::fixed` + @e arithmetic = `fxd::fixed`
    template<typename Fxd,
             typename Other>
    requires (fxd::is_fixed_point_v<Fxd> && is_arithmetic_v<Other>)
    struct common_type<Fxd, Other> {
        /// @cond
        using type = Fxd;
        /// @endcond
    };


    /// Specialization: @e arithmetic + `fxd::fixed` = `fxd::fixed`
    template<typename Other,
             typename Fxd>
    requires (is_arithmetic_v<Other> && fxd::is_fixed_point_v<Fxd>)
    struct common_type<Other, Fxd> {
        /// @cond
        using type = Fxd;
        /// @endcond
    };

}



#endif
