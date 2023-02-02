/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_CONCEPTS_HPP
#define LIBFXD_CONCEPTS_HPP


#include "traits.hpp"


namespace fxd {

    /// Concept to match any `fxd::fixed`
    template<typename T>
    concept fixed_point = is_fixed_point_v<T>;


    /// Concept to match only unsigned `fxd::fixed`
    template<typename T>
    concept unsigned_fixed_point = is_unsigned_fixed_point_v<T>;


    /// Concept to match only signed `fxd::fixed`
    template<typename T>
    concept signed_fixed_point = is_signed_fixed_point_v<T>;


}


#endif
