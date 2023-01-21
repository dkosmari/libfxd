#ifndef LIBFXD_CASTING_HPP
#define LIBFXD_CASTING_HPP

#include <type_traits>

#include "concepts.hpp"
#include "utils-shift.hpp"


namespace fxd {


    template<int Int,
             int Frac,
             typename Raw = select_int_t<Int + Frac>,
             fixed_point Src>
    constexpr
    fixed<Int, Frac, Raw>
    fixed_cast(const Src& src)
        noexcept
    {
        Raw raw = utils::shift::shl(Raw(src.raw_value),
                                    Frac - Src::fractional_bits);
        return fixed<Int, Frac, Raw>::from_raw(raw);
    }



    template<int Int,
             int Frac,
             typename Raw = select_uint_t<Int + Frac>,
             fixed_point Src>
    constexpr
    fixed<Int, Frac, Raw>
    ufixed_cast(const Src& src)
        noexcept
    {
        return fixed_cast<Int, Frac, Raw>(src);
    }


}


#endif
