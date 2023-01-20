#ifndef LIBFXD_UTILS_HPP
#define LIBFXD_UTILS_HPP


#include <cfenv>
#include <new>


#if defined(__GNUC__) || defined(__clang__)
#define ALWAYS_INLINE __attribute__((always_inline))
#else
#define ALWAYS_INLINE inline
#endif


namespace fxd::utils {


    // helper function to kill constant propagation optimizations
    template<typename T>
    inline
    T
    opacify(T a)
        noexcept
    {
        return *std::launder(&a);
    }



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

}



#endif
