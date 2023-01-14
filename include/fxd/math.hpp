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
        if (a > b)
            return a - b;
        else
            return 0;
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



    // Babylonian/Heron/Newton-Raphson method

    template<fxd::fixed_point Fxd>
    Fxd
    sqrt(Fxd x)
    {
        if (!x)
            return x;

        int i = 0;
        Fxd a = x;
        Fxd old_a;

        do {
            if (++i > Fxd::bits)
                return a;

            // TODO: should ensure this division is rounding up
            Fxd b = x / a;

            if (a > b) // a is always the lower bound
                std::swap(a, b);

            old_a = a;

            a = (a + b) / 2;

            if (!a)
                return a;

        } while (old_a != a);

        return a;
    }


}


#endif
