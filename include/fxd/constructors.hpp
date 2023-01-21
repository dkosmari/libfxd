#ifndef LIBFXD_CONSTRUCTORS_HPP
#define LIBFXD_CONSTRUCTORS_HPP

#include <cfenv>
#include <cmath>
#include <concepts>

#include "fixed.hpp"
#include "utils-shift.hpp"


namespace fxd {


    template<int Int,
             int Frac,
             typename Raw>
    template<std::integral I>
    constexpr
    fixed<Int, Frac, Raw>::fixed(I i)
        noexcept :
        raw_value{utils::shift::shl<raw_type>(i, frac_bits)}
    {}


    template<int Int,
             int Frac,
             typename Raw>
    template<std::floating_point Flt>
    constexpr
    fixed<Int, Frac, Raw>::fixed(Flt f)
        noexcept
    {
        const auto scaled_f = std::ldexp(f, frac_bits);
        const auto rounded_f = std::rint(scaled_f);
        raw_value = static_cast<raw_type>(rounded_f);
    }


    template<int Int,
             int Frac,
             typename Raw>
    constexpr
    fixed<Int, Frac, Raw>
    fixed<Int, Frac, Raw>::from_raw(Raw val)
        noexcept
    {
        fixed result;
        result.raw_value = val;
        return result;
    }


}


#endif
