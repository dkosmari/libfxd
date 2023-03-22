#ifndef ROUNDING_HPP
#define ROUNDING_HPP

#include <cfenv>
#include <cmath>
#include <concepts>


namespace rounding {


    // round up at place 2^(-exp)
    template<std::floating_point F>
    F
    ceil(F f,
         int exp)
        noexcept
    {
        F f_scaled = std::ldexp(f, exp);
        F f_scaled_rounded = std::ceil(f_scaled);
        F f_rounded = std::ldexp(f_scaled_rounded, -exp);
        return f_rounded;
    }


    // round down at place 2^(-exp)
    template<std::floating_point F>
    F
    floor(F f,
          int exp)
        noexcept
    {
        F f_scaled = std::ldexp(f, exp);
        F f_scaled_rounded = std::floor(f_scaled);
        F f_rounded = std::ldexp(f_scaled_rounded, -exp);
        return f_rounded;
    }


    // round to zero a place 2^(-exp)
    template<std::floating_point F>
    F
    trunc(F f,
          int exp)
        noexcept
    {
        F f_scaled = std::ldexp(f, exp);
        F f_scaled_rounded = std::trunc(f_scaled);
        F f_rounded = std::ldexp(f_scaled_rounded, -exp);
        return f_rounded;
    }


    namespace up {

        template<std::floating_point F>
        F
        mul(F a,
            F b,
            int exp)
            noexcept
        {
            int old = std::fegetround();
            std::fesetround(FE_UPWARD);
            volatile F c = a * b;
            std::fesetround(old);
            return ceil(c, exp);
        }


        template<std::floating_point F>
        F
        div(F a,
            F b,
            int exp)
            noexcept
        {
            int old = std::fegetround();
            std::fesetround(FE_UPWARD);
            volatile F c = a / b;
            std::fesetround(old);
            return ceil(c, exp);
        }

    }


    namespace down {

        template<std::floating_point F>
        F
        mul(F a,
            F b,
            int exp)
            noexcept
        {
            int old = std::fegetround();
            std::fesetround(FE_DOWNWARD);
            volatile F c = a * b;
            std::fesetround(old);
            return floor(c, exp);
        }


        template<std::floating_point F>
        F
        div(F a,
            F b,
            int exp)
            noexcept
        {
            int old = std::fegetround();
            std::fesetround(FE_DOWNWARD);
            volatile F c = a / b;
            std::fesetround(old);
            return floor(c, exp);
        }

    }


    namespace zero {

        template<std::floating_point F>
        F
        mul(F a,
            F b,
            int exp)
            noexcept
        {
            int old = std::fegetround();
            std::fesetround(FE_TOWARDZERO);
            volatile F c = a * b;
            std::fesetround(old);
            return trunc(c, exp);
        }


        template<std::floating_point F>
        F
        div(F a,
            F b,
            int exp)
            noexcept
        {
            int old = std::fegetround();
            std::fesetround(FE_TOWARDZERO);
            volatile F c = a / b;
            std::fesetround(old);
            return trunc(c, exp);
        }
    }


}


#endif
