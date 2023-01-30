#ifndef ROUNDER_HPP
#define ROUNDER_HPP

#include <cfenv>


#if defined(__clang__) && __clang_major__ >= 12 && !defined(__FAST_MATH__)
#pragma STDC FENV_ACCESS ON
#endif


template<int Mode>
struct rounder {

    int old_mode;

    rounder()
    noexcept :
    old_mode{std::fegetround()}
    {
        std::fesetround(Mode);
    }

    ~rounder()
    noexcept
    {
        if (old_mode >= 0)
            std::fesetround(old_mode);
    }

};


using round_zero = rounder<FE_TOWARDZERO>;
using round_nearest = rounder<FE_TONEAREST>;
using round_up = rounder<FE_UPWARD>;
using round_down = rounder<FE_DOWNWARD>;


#endif
