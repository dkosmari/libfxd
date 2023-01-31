/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_IMPL_ERROR_HPP
#define LIBFXD_IMPL_ERROR_HPP

namespace fxd::impl {

    enum class error {
        underflow,
        overflow,
        not_a_number
    };

}

#endif
