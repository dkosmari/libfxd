#ifndef LIBFXD_MATH_HPP
#define LIBFXD_MATH_HPP

#include "fixed.hpp"

#include "concepts.hpp"
#include "limits.hpp"


namespace fxd {


    template<fixed_point Fxd>
    constexpr
    Fxd
    abs(Fxd f)
        noexcept
    {
        return Fxd::from_raw(std::abs(f.raw_value));
    }



    template<fixed_point Fxd>
    constexpr
    Fxd
    fdim(Fxd a, Fxd b)
        noexcept
    {
        if (a >= b)
            return a - b;
        else
            return b - a;
    }



    template<fixed_point Fxd>
    constexpr
    Fxd
    nextafter(Fxd from, Fxd to)
        noexcept
    {
        constexpr Fxd e = std::numeric_limits<Fxd>::epsilon();
        if (from < to)
            return from + e;
        if (from > to)
            return from - e;
        return to;
    }





}


#endif
