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
    struct is_fixed_point : std::false_type {};
    /// @endcond

    /// Test if a type is `fxd::fixed`.
    template<int Int,
             int Frac,
             typename Raw>
    struct is_fixed_point<fixed<Int, Frac, Raw>> :
        std::true_type {};


    /// @cond
    template<typename T>
    struct is_signed_fixed_point : std::false_type {};
    /// @endcond

    /// Test if a type is signed `fxd::fixed`.
    template<int Int,
             int Frac,
             std::signed_integral Raw>
    struct is_signed_fixed_point<fixed<Int, Frac, Raw>> :
        std::true_type {};



    /// @cond
    template<typename T>
    struct is_unsigned_fixed_point : std::false_type {};
    /// @endcond

    /// Test if a type is unsigned `fxd::fixed`.
    template<int Int,
             int Frac,
             std::unsigned_integral Raw>
    struct is_unsigned_fixed_point<fixed<Int, Frac, Raw>> :
        std::true_type {};


    /// Helper template variable for `fxd::is_fixed_point`
    template<typename T>
    inline constexpr
    bool is_fixed_point_v          = is_fixed_point<T>::value;

    /// Helper template variable for `fxd::is_signed_fixed_point`
    template<typename T>
    inline constexpr
    bool is_signed_fixed_point_v   = is_signed_fixed_point<T>::value;

    /// Helper template variable for `fxd::is_unsigned_fixed_point`
    template<typename T>
    inline constexpr
    bool is_unsigned_fixed_point_v = is_unsigned_fixed_point<T>::value;



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


    /// Specialization: `fxd::fixed` + @e anything = `fxd::fixed`
    template<typename Fxd,
             typename Other>
    requires (fxd::is_fixed_point_v<Fxd> && is_arithmetic_v<Other>)
    struct common_type<Fxd, Other> {
        /// @cond
        using type = Fxd;
        /// @endcond
    };


    /// Specialization: @e anything + `fxd::fixed` = `fxd::fixed`
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
