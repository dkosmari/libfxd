#ifndef LIBFXD_FIXED_HPP
#define LIBFXD_FIXED_HPP

#include <concepts>
#include <limits>

#include "types.hpp"
#include "utils.hpp"


namespace fxd {


    template<int Int,
             int Frac,
             typename T = select_int_t<Int + Frac>>
    struct fixed {

        using raw_type = T;
        static constexpr int raw_bits = utils::type_width<raw_type>;
        static constexpr int int_bits = Int;
        static constexpr int frac_bits = Frac;
        static constexpr int bits = int_bits + frac_bits;

        static_assert(raw_bits >= bits);
        static_assert(bits > 0);
        static_assert(2 * raw_bits >= int_bits);
        static_assert(2 * raw_bits >= frac_bits);

        // floating point type that can fully represent this fixed point type
        using float_type = select_float_t<bits>;

        /*
         * The actual value is stored as a bitfield.
         * By default, the bitfield has the full size of the underlying type,
         * so it has no runtime cost. If the bits are limited, the compiler
         * will generate the appropriate shift and mask operations.
         */
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

        fixed& operator =(const fixed&) noexcept = default;

        template<std::integral I>
        constexpr fixed& operator =(I i) noexcept;

        template<std::floating_point Flt>
        constexpr fixed& operator = (Flt f) noexcept;


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
        explicit
        operator F() const noexcept;


        // TODO: might be a problem when there are no integral bits
        static constexpr fixed one = 1;


    };



    template<int Int,
             int Frac>
    using ufixed = fixed<Int, Frac, select_uint_t<Int + Frac>>;



    // deduction guides

    template<std::integral I>
    fixed(I i) -> fixed<8 * sizeof(I), 0, I>;

}



#endif
