#ifndef LIBFXD_CASTING_HPP
#define LIBFXD_CASTING_HPP

#include <type_traits>

#include "fixed.hpp"

#include "concepts.hpp"
#include "utils-shift.hpp"


namespace fxd {


    template<int Int,
             int Frac,
             typename T = select_int_t<Int + Frac>,
             fixed_point Src>
    constexpr
    fixed<Int, Frac, T>
    fixed_cast(const Src& src)
        noexcept
    {
        T raw = utils::shift::shl(T(src.raw_value),
                                  Frac - Src::fractional_bits);
        return fixed<Int, Frac, T>::from_raw(raw);
    }



    template<int Int,
             int Frac,
             typename T = select_uint_t<Int + Frac>,
             fixed_point Src>
    constexpr
    fixed<Int, Frac, T>
    ufixed_cast(const Src& src)
        noexcept
    {
        return fixed_cast<Int, Frac, T>(src);
    }


}


#endif
