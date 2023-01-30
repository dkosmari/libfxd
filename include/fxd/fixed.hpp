/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_FIXED_HPP
#define LIBFXD_FIXED_HPP

#include <compare>
#include <concepts>
#include <limits>

#include "types.hpp"


namespace fxd {


    template<int Int,
             int Frac,
             typename Raw = select_int_t<Int + Frac>>
    struct fixed {

        using raw_type = Raw;
        static_assert(std::numeric_limits<Raw>::is_specialized,
                      "raw type is not usable, lacks std::numeric_limits support");

        static constexpr int raw_bits = type_width<raw_type>;
        static constexpr int int_bits = Int;
        static constexpr int frac_bits = Frac;
        static constexpr int bits = int_bits + frac_bits;

        static_assert(raw_bits >= bits);
        static_assert(bits > 0);
        static_assert(int_bits <= 2 * raw_bits);
        static_assert(frac_bits <= 2 * raw_bits);


        // floating point type that can fully represent this fixed point type
        using float_type = select_float_t<bits - std::numeric_limits<raw_type>::is_signed>;

        // The raw value is stored as a bitfield.
        raw_type raw_value : bits;



        // constructors

        constexpr fixed() noexcept = default;
        constexpr fixed(const fixed&) noexcept = default;

        template<std::integral I>
        constexpr fixed(I i) noexcept;

        template<std::floating_point Flt>
        constexpr fixed(Flt f) noexcept;


        // named constructors

        static constexpr fixed from_raw(raw_type val) noexcept;


        // assignment
        constexpr fixed& operator =(const fixed&) noexcept = default;


        // Comparison

        constexpr
        bool operator ==(const fixed& other)
            const noexcept = default;

        constexpr
        std::strong_ordering operator <=>(const fixed& other)
            const noexcept = default;


        // conversion

        explicit constexpr
        operator bool() const noexcept;

        template<std::integral I>
        explicit constexpr
        operator I() const noexcept;

        template<std::floating_point F>
        explicit constexpr
        operator F() const noexcept;


    };



    template<int Int,
             int Frac>
    using ufixed = fixed<Int, Frac, select_uint_t<Int + Frac>>;


}



#endif
