/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_UTILS_HPP
#define LIBFXD_UTILS_HPP

#if 1 && (defined(__GNUC__) || defined(__clang__))
#define ALWAYS_INLINE __attribute__((always_inline))
#else
#define ALWAYS_INLINE inline
#endif


#endif
