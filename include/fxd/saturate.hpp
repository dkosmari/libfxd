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

#include "impl/error.hpp"
#include "impl/safe-includes.hpp"


namespace fxd::saturate {


    template<fxd::fixed_point Fxd>
    [[nodiscard]]
    Fxd
    handler(impl::error e)
        noexcept
    {
        switch (e) {
            case impl::error::underflow:
                return std::numeric_limits<Fxd>::lowest();
            case impl::error::overflow:
                return std::numeric_limits<Fxd>::max();
            case impl::error::not_a_number:
                std::raise(SIGFPE);
        }
        return 0;
    }


#define LIBFXD_INCLUDING_IMPL_SAFE_HPP
#include "impl/safe.hpp"
#undef LIBFXD_INCLUDING_IMPL_SAFE_HPP


} // namespace fxd::saturate


#endif
