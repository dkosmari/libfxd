#ifndef LIBFXD_CONVERSIONS_HPP
#define LIBFXD_CONVERSIONS_HPP

#include <cmath>
#include <concepts>

#include "fixed.hpp"

#include "utils-shift.hpp"
#include "concepts.hpp"


namespace fxd {


    template<int Int,
             int Frac,
             typename T>
    constexpr
    fixed<Int, Frac, T>::operator bool()
        const noexcept
    {
        return raw_value;
    }


    template<int Int,
             int Frac,
             typename T>
    template<std::integral I>
    constexpr
    fixed<Int, Frac, T>::operator I()
        const noexcept
    {
        return utils::shift::shrz(raw_value, frac_bits);
    }


    template<int Int,
             int Frac,
             typename T>
    template<std::floating_point Flt>
    fixed<Int, Frac, T>::operator Flt()
        const noexcept
    {
        utils::rounder r;
        using std::ldexp;
        return ldexp(static_cast<Flt>(utils::opacify(raw_value)), -frac_bits);
    }


    template<fixed_point Fxd>
    requires(!std::is_void_v<typename Fxd::float_type>)
    Fxd::float_type
    to_float(Fxd f)
        noexcept
    {
        using Flt = Fxd::float_type;
        static_assert(!std::is_void_v<Flt>);
        return static_cast<Flt>(f);
    }

}


#endif
