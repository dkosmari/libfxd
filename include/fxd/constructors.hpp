/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_CONSTRUCTORS_HPP
#define LIBFXD_CONSTRUCTORS_HPP

#include <cfenv>
#include <cmath>
#include <concepts>
#include <type_traits>

#include "fixed.hpp"

#include "detail/bias.hpp"
#include "detail/opacify.hpp"
#include "detail/shift.hpp"


namespace fxd {


    /**
     * The `i` value will be shifted (and rounded to zero, if negative)
     * if needed.
     */
    template<int Int,
             int Frac,
             typename Raw>
    template<std::integral I>
    [[nodiscard]]
    constexpr
    fixed<Int, Frac, Raw>::fixed(I i)
        noexcept
    {
        if constexpr (frac_bits < 0) {

            // Shifting right.
            // Bias negative values.
            if (i < 0)
                i += detail::make_bias_for(-frac_bits, i);

            raw_value = detail::shr_real(i, -frac_bits);

        } else {

            // Shifting left.
            // Use max bits to minimize spurious overflows.
            using IWide = detail::max_int_for<raw_type>;
            raw_value = detail::shl_real<IWide>(i, frac_bits);

        }

    }



    template<int Int,
             int Frac,
             typename Raw>
    template<std::floating_point Flt>
    constexpr
    fixed<Int, Frac, Raw>::fixed(Flt f)
        noexcept
    {
        const auto scaled_f = std::ldexp(f, frac_bits);
        raw_value = static_cast<raw_type>(scaled_f);
    }



    template<int Int, int Frac, typename Raw>
    template<int Int2, int Frac2, typename Raw2>
    requires (detail::is_safe_conversion_v<Int2, Frac2, Raw2,
                                           Int, Frac, Raw>)
    constexpr
    fixed<Int, Frac, Raw>::fixed(const fixed<Int2, Frac2, Raw2>& other)
        noexcept :
        raw_value{fixed_cast<fixed<Int, Frac, Raw>>(other).raw_value}
    {}



    /**
     * All excess bits of `val` will be truncated. Use this when you know the exact
     * bit representation needed for the `fixed` object.
     */
    template<int Int,
             int Frac,
             typename Raw>
    [[nodiscard]]
    constexpr
    fixed<Int, Frac, Raw>
    fixed<Int, Frac, Raw>::from_raw(Raw val)
        noexcept
    {
        fixed result;
        result.raw_value = static_cast<Raw>(val);
        return result;
    }


}


#endif
