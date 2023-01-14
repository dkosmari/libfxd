#ifndef LIBFXD_UTILS_HPP
#define LIBFXD_UTILS_HPP


#include <cfenv>


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
#if defined(__GNUC__ ) || defined(__clang__)
        asm volatile ("" : "+rmi" (a) : "rmi" (a));
        return a;
#else
        volatile T aa = a;
        return aa;
#endif
    }



#if defined(__clang__) && __clang_major__ >= 12 && !defined(__FAST_MATH__)
#pragma STDC FENV_ACCESS ON
#endif


    struct rounder {

        int mode;

        rounder()
        noexcept :
            mode{std::fegetround()}
        {
            std::fesetround(FE_TOWARDZERO);
        }

        ~rounder()
            noexcept
        {
            if (mode >= 0)
                std::fesetround(mode);
        }

    };

}



#endif
