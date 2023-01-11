#ifndef LIBFXD_CONSTRUCTORS_HPP
#define LIBFXD_CONSTRUCTORS_HPP

#include <cmath>

#include "fixed.hpp"

#include "utils-shift.hpp"


namespace fxd {


    template<int Int,
             int Frac,
             typename T>
    template<std::integral I>
    constexpr
    fixed<Int, Frac, T>::fixed(I i)
        noexcept :
        raw_value(utils::shift::shl<raw_type>(i, frac_bits))
    {}


    template<int Int,
             int Frac,
             typename T>
    template<std::floating_point Flt>
    constexpr
    fixed<Int, Frac, T>::fixed(Flt f)
        noexcept
    {
        // note: no need to control rounding mode here, since
        // conversion to int is always rounded to zero
        using std::ldexp;
        raw_value = static_cast<raw_type>(ldexp(f, frac_bits));
    }


    template<int Int,
             int Frac,
             typename T>
    constexpr
    fixed<Int, Frac, T>
    fixed<Int, Frac, T>::from_raw(T val)
        noexcept
    {
        fixed result;
        result.raw_value = val;
        return result;
    }


}


#endif
