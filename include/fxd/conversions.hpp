#ifndef LIBFXD_CONVERSIONS_HPP
#define LIBFXD_CONVERSIONS_HPP

#include <cmath>
#include <concepts>

#include "fixed.hpp"

#include "utils.hpp"
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
        return utils::shrz(raw_value, frac_bits);
    }


    template<int Int,
             int Frac,
             typename T>
    template<std::floating_point Flt>
    constexpr
    fixed<Int, Frac, T>::operator Flt()
        const noexcept
    {
        using std::ldexp;
        return ldexp(static_cast<Flt>(raw_value), -frac_bits);
    }


    template<fixed_point Fxd>
    constexpr
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
