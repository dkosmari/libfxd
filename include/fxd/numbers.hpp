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


namespace std::numbers {

#ifdef __cpp_lib_constexpr_cmath
#    define LIBFXD_CONSTEXPR constexpr
#else
#    define LIBFXD_CONSTEXPR const
#endif

#define LIBFXD_NUMBER(x)                                        \
    template<fxd::fixed_point F>                                \
    inline LIBFXD_CONSTEXPR                                     \
    F x<F> = F{std::numbers:: x <typename F::float_type>}


    LIBFXD_NUMBER(e_v);
    LIBFXD_NUMBER(egamma_v);
    LIBFXD_NUMBER(inv_pi_v);
    LIBFXD_NUMBER(inv_sqrt3_v);
    LIBFXD_NUMBER(inv_sqrtpi_v);
    LIBFXD_NUMBER(ln10_v);
    LIBFXD_NUMBER(ln2_v);
    LIBFXD_NUMBER(log10e_v);
    LIBFXD_NUMBER(log2e_v);
    LIBFXD_NUMBER(phi_v);
    LIBFXD_NUMBER(pi_v);
    LIBFXD_NUMBER(sqrt2_v);
    LIBFXD_NUMBER(sqrt3_v);


#undef LIBFXD_NUMBER
#undef LIBFXD_CONSTEXPR

}

#endif
