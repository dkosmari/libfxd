/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_FIXED_HPP
#define LIBFXD_FIXED_HPP

//#include <compare>
#include <concepts>
#include <limits>

#include "detail/types.hpp"

/// This is the namespace where the entire library is defined.
namespace fxd {

    /**
     * @brief The fixed-point class template.
     *
     * This class encapsulates an integer so it supports the arithmetic operations.  It
     * mimics floating-point types, but direct access to the internal integer can be done
     * through the `.raw_value` member variable.
     *
     * @tparam Int How many integral bits to use.
     * @tparam Frac How many fractional bits to use.
     * @tparam Raw The integral type that will be used to store the value. By
     * default, it's a signed integer with at least `Int + Frac` bits.
     *
     * @sa `ufixed`.
     */
    template<int Int,
             int Frac,
             typename Raw = detail::select_int_t<Int + Frac>
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

        /// How many bits are used for storage (can be larger than `bits`).
        static constexpr int raw_bits =
            detail::type_width<raw_type>;


        static_assert(raw_bits >= bits);
        static_assert(bits > 0);
        static_assert(int_bits <= 2 * raw_bits);
        static_assert(frac_bits <= 2 * raw_bits);

        /// The raw value, stored as a bitfield.
        raw_type raw_value : bits;



        // constructors

        /// Defaulted constructor.
        constexpr fixed() noexcept = default;

        // Defaulted copy constructor.
        // constexpr fixed(const fixed&) noexcept = default;

        /// Conversion from integer.
        template<std::integral I>
        constexpr fixed(I i) noexcept;

        /// Conversion from floating-point.
        template<std::floating_point Flt>
        constexpr fixed(Flt f) noexcept;


        /// Conversion from another fixed-point
        template<int Int2, int Frac2, typename Raw2>
        requires (detail::is_safe_conversion_v<Int2, Frac2, Raw2,
                                               Int, Frac, Raw>)
        constexpr fixed(const fixed<Int2, Frac2, Raw2>& other) noexcept;


        // named constructors

        /// Constructs a `fixed` from any raw bit representation (no conversion.)
        static constexpr fixed from_raw(raw_type val) noexcept;


        // Defaulted copy assignment.
        //constexpr fixed& operator =(const fixed&) noexcept = default;


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

        /// Conversion to another fixed-point.
        template<int Int2, int Frac2, typename Raw2>
        explicit
        constexpr
        operator fixed<Int2, Frac2, Raw2>() const noexcept;

    };


    /// Alias for creating `unsigned fxd::fixed`.
    template<int Int,
             int Frac>
    using ufixed = fixed<Int, Frac, detail::select_uint_t<Int + Frac>>;


}

#endif
