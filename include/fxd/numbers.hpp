/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_NUMBERS_HPP
#define LIBFXD_NUMBERS_HPP

#include <numbers>

#include "concepts.hpp"


/// Specialization for mathematical constants.
namespace std::numbers {

#ifdef __cpp_lib_constexpr_cmath
#    define LIBFXD_CONSTEXPR constexpr
#else
#    define LIBFXD_CONSTEXPR const
#endif

#define LIBFXD_NUMBER(c)                                                \
    template<fxd::fixed_point Fxd>                                      \
    inline LIBFXD_CONSTEXPR                                             \
    Fxd c<Fxd> = Fxd{std::numbers:: c <typename std::numeric_limits<Fxd>::float_type>}

    /// Fixed-point constant for @f$e@f$. @hideinitializer
    LIBFXD_NUMBER(e_v);

    /// Fixed-point constant for @f$\gamma@f$. @hideinitializer
    LIBFXD_NUMBER(egamma_v);

    /// Fixed-point constant for @f$1 \over \pi@f$ @hideinitializer
    LIBFXD_NUMBER(inv_pi_v);

    /// Fixed-point constant for @f$1 \over \sqrt 3@f$ @hideinitializer
    LIBFXD_NUMBER(inv_sqrt3_v);

    /// Fixed-point constant for @f$1 \over \sqrt \pi@f$ @hideinitializer
    LIBFXD_NUMBER(inv_sqrtpi_v);

    /// Fixed-point constant for @f$\ln 10@f$ @hideinitializer
    LIBFXD_NUMBER(ln10_v);

    /// Fixed-point constant for @f$\ln 2@f$ @hideinitializer
    LIBFXD_NUMBER(ln2_v);

    /// Fixed-point constant for @f$\log _ {10} e@f$ @hideinitializer
    LIBFXD_NUMBER(log10e_v);

    /// Fixed-point constant for @f$\log _ 2 e@f$ @hideinitializer
    LIBFXD_NUMBER(log2e_v);

    /// Fixed-point constant for @f$\phi@f$ @hideinitializer
    LIBFXD_NUMBER(phi_v);

    /// Fixed-point constant for @f$\pi@f$ @hideinitializer
    LIBFXD_NUMBER(pi_v);

    /// Fixed-point constant for @f$\sqrt 2@f$ @hideinitializer
    LIBFXD_NUMBER(sqrt2_v);

    /// Fixed-point constant for @f$\sqrt 3@f$ @hideinitializer
    LIBFXD_NUMBER(sqrt3_v);


#undef LIBFXD_NUMBER
#undef LIBFXD_CONSTEXPR

}

#endif
