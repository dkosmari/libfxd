/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_ERROR_HPP
#define LIBFXD_ERROR_HPP

namespace fxd {

    /// Error values reported by `fxd::expect` function.
    enum class error {
        underflow,
        overflow,
        not_a_number
    };

}

#endif
