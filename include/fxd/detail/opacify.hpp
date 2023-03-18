/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_DETAIL_OPACIFY_HPP
#define LIBFXD_DETAIL_OPACIFY_HPP

#include <new> // launder()

namespace fxd::detail {


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
