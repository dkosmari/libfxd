#ifndef LIBFXD_UTILS_SQRT_HPP
#define LIBFXD_UTILS_SQRT_HPP

#include <algorithm>

#include "fixed.hpp"

#include "concepts.hpp"
#include "operators.hpp"


namespace fxd::utils::sqrt {


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
