/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_UTILS_OPACIFY_HPP
#define LIBFXD_UTILS_OPACIFY_HPP

#include <new> // launder()

namespace fxd::utils {


    // helper function to kill constant propagation optimizations
    template<typename T>
    inline
    T
    opacify(T a)
        noexcept
    {
        return *std::launder(&a);
    }


}

#endif
