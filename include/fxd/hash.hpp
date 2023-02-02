/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_HASH_HPP
#define LIBFXD_HASH_HPP

#include <functional>

#include "concepts.hpp"


namespace std {

    /// Specialization: `std::hash<fxd::fixed>`.
    template<fxd::fixed_point Fxd>
    struct hash<Fxd> {

        /// Calculate a hash value.
        constexpr
        size_t
        operator ()(Fxd f)
            const noexcept
        {
            using H = hash<typename Fxd::raw_type>;
            return H{}(f.raw_value);
        }

    };

}


#endif
