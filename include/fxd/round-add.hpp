#ifndef LIBFXD_ROUND_ADD_HPP
#define LIBFXD_ROUND_ADD_HPP

#include "fixed.hpp"

#include "concepts.hpp"
#include "utils.hpp"


// adding never rounds

namespace fxd::round {

    template<fixed_point Fxd>
    ALWAYS_INLINE
    constexpr
    Fxd
    add(Fxd a,
        Fxd b)
        noexcept
    {
        return Fxd::from_raw(a.raw_value + b.raw_value);
    }


    namespace zero { using fxd::round::add; }
    namespace up   { using fxd::round::add; }
    namespace down { using fxd::round::add; }

}



#endif
