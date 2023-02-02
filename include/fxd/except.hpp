/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_EXCEPT_HPP
#define LIBFXD_EXCEPT_HPP

#include <cstdlib>
#include <stdexcept>

#include "concepts.hpp"
#include "impl/error.hpp"
#include "impl/safe-includes.hpp"

/**
 * @file except.hpp
 *
 * @brief Defines range-checked functions that throw exceptions on error.
 *
 */


/// Provides throwing range-checked functions.
namespace fxd::except {


    /**
     * @brief Error handler that throws exceptions on error.
     *
     * Possible exceptions are:
     * @li `std::underflow_error` on underflows;
     * @li `std::overflow_error` on overflows;
     * @li `std::invalid_argument` when NaN would be generated;
     */
    template<fxd::fixed_point Fxd>
    [[noreturn]]
    Fxd
    handler(impl::error e)
    {
        switch (e) {
            case impl::error::underflow:
                throw std::underflow_error{"underflow"};
            case impl::error::overflow:
                throw std::overflow_error{"overflow"};
            case impl::error::not_a_number:
                throw std::invalid_argument{"not a number"};
            default:
                throw std::logic_error{"unknown error"};
        }
    }



#define LIBFXD_INCLUDING_IMPL_SAFE_HPP
#include "impl/safe.hpp"
#undef LIBFXD_INCLUDING_IMPL_SAFE_HPP

    /**
     * @include impl/safe.hpp
     */

} // namespace fxd::except


#endif
