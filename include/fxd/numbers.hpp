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


#define LIBFXD_CONSTANT(x)                                       \
    template<fxd::fixed_point F>                                \
    inline constexpr                                            \
    F x<F> = F{std::numbers:: x <typename F::float_type>}


    LIBFXD_CONSTANT(e_v);
    LIBFXD_CONSTANT(egamma_v);
    LIBFXD_CONSTANT(inv_pi_v);
    LIBFXD_CONSTANT(inv_sqrt3_v);
    LIBFXD_CONSTANT(inv_sqrtpi_v);
    LIBFXD_CONSTANT(ln10_v);
    LIBFXD_CONSTANT(ln2_v);
    LIBFXD_CONSTANT(log10e_v);
    LIBFXD_CONSTANT(log2e_v);
    LIBFXD_CONSTANT(phi_v);
    LIBFXD_CONSTANT(pi_v);
    LIBFXD_CONSTANT(sqrt2_v);
    LIBFXD_CONSTANT(sqrt3_v);


#undef LIBFXD_CONSTANT


}

#endif
