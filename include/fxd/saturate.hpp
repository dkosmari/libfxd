/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_SATURATE_HPP
#define LIBFXD_SATURATE_HPP

#include <csignal>

#include "concepts.hpp"
#include "error.hpp"
#include "utils-safe-includes.hpp"


namespace fxd::saturate {


    template<fxd::fixed_point Fxd>
    [[nodiscard]]
    Fxd
    handler(error e)
        noexcept
    {
        switch (e) {
            case error::underflow:
                return std::numeric_limits<Fxd>::lowest();
            case error::overflow:
                return std::numeric_limits<Fxd>::max();
            case error::not_a_number:
                std::raise(SIGFPE);
        }
        return 0;
    }


#define LIBFXD_INCLUDING_UTILS_SAFE_HPP_IMPL
#include "utils-safe.hpp"
#undef LIBFXD_INCLUDING_UTILS_SAFE_HPP_IMPL


} // namespace fxd::saturate


#endif
