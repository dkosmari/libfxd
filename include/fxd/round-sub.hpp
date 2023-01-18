#ifndef LIBFXD_ROUND_SUB_HPP
#define LIBFXD_ROUND_SUB_HPP

#include "fixed.hpp"

#include "concepts.hpp"
#include "utils.hpp"


// subtracting never rounds

namespace fxd::round {

    template<fixed_point Fxd>
    ALWAYS_INLINE
    constexpr
    Fxd
    sub(Fxd a,
        Fxd b)
        noexcept
    {
        return Fxd::from_raw(a.raw_value - b.raw_value);
    }


    namespace zero { using fxd::round::sub; }
    namespace up   { using fxd::round::sub; }
    namespace down { using fxd::round::sub; }

}



#endif
