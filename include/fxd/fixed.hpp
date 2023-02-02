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

#include "impl/types.hpp"

/// This is the namespace where the entire library is defined.
namespace fxd {

    /**
     * @brief The fixed-point class template.
     *
     * This class encapsulates an integer so it supports the arithmetic operations.  It
     * mimics floating-point types, but direct access to the internal integer can be done
     * through the `.raw_value` member variable.
     */
    template<int Int, /// How many integral bits to use.
             int Frac, /// How many fractional bits to use.
             typename Raw = impl::select_int_t<Int + Frac> /// The integral type that will be used.
             >
    struct fixed {

        /// The integral type that will hold the raw representation of the value.
        using raw_type = Raw;
        static_assert(std::numeric_limits<Raw>::is_specialized,
                      "raw type is not usable, lacks std::numeric_limits support");

        /// How many integral bits were requested.
        static constexpr int int_bits = Int;

        /// How many fractional bits were requested.
        static constexpr int frac_bits = Frac;

        /// How many bits were requested in total.
        static constexpr int bits = int_bits + frac_bits;

        /// How many bits are used (can be larger than `bits`).
        static constexpr int raw_bits =
            impl::type_width<raw_type>;


        static_assert(raw_bits >= bits);
        static_assert(bits > 0);
        static_assert(int_bits <= 2 * raw_bits);
        static_assert(frac_bits <= 2 * raw_bits);


        /// A floating-point type that can fully represent this fixed-point.
        using float_type =
            impl::select_float_t<bits - std::numeric_limits<raw_type>::is_signed>;

        /// The raw value, stored as a bitfield.
        raw_type raw_value : bits;



        // constructors

        /// Defaulted constructor.
        constexpr fixed() noexcept = default;

        /// Defaulted copy constructor.
        constexpr fixed(const fixed&) noexcept = default;

        /// Conversion from integer.
        template<std::integral I>
        constexpr fixed(I i) noexcept;

        /// Conversion from floating-point.
        template<std::floating_point Flt>
        constexpr fixed(Flt f) noexcept;


        // named constructors

        /// Constructs a `fixed` from any raw bit representation (no conversion.)
        static constexpr fixed from_raw(raw_type val) noexcept;


        /// Defaulted copy assignment.
        constexpr fixed& operator =(const fixed&) noexcept = default;


        // Comparison

        /// Defaulted `==` operator.
        constexpr
        bool operator ==(const fixed& other)
            const noexcept = default;

        /// Defaulted `<=>` operator.
        constexpr
        std::strong_ordering operator <=>(const fixed& other)
            const noexcept = default;


        // conversion

        /// Check if non-zero.
        explicit constexpr
        operator bool() const noexcept;

        /// Conversion to integer.
        template<std::integral I>
        explicit constexpr
        operator I() const noexcept;

        /// Conversion to floating-point.
        template<std::floating_point F>
        explicit constexpr
        operator F() const noexcept;


    };


    /// Alias for creating unsigned `fxd::fixed`.
    template<int Int,
             int Frac>
    using ufixed = fixed<Int, Frac, impl::select_uint_t<Int + Frac>>;


}



#endif
