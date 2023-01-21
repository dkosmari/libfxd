#ifndef LIBFXD_NUMBERS_HPP
#define LIBFXD_NUMBERS_HPP

#include <numbers>

#include "concepts.hpp"


namespace std::numbers {


#define FIXED_CONSTANT(x)                                       \
    template<fxd::fixed_point F>                                \
    inline constexpr                                            \
    F x<F> = F{std::numbers:: x <typename F::float_type>}


    FIXED_CONSTANT(e_v);
    FIXED_CONSTANT(egamma_v);
    FIXED_CONSTANT(inv_pi_v);
    FIXED_CONSTANT(inv_sqrt3_v);
    FIXED_CONSTANT(inv_sqrtpi_v);
    FIXED_CONSTANT(ln10_v);
    FIXED_CONSTANT(ln2_v);
    FIXED_CONSTANT(log10e_v);
    FIXED_CONSTANT(log2e_v);
    FIXED_CONSTANT(phi_v);
    FIXED_CONSTANT(pi_v);
    FIXED_CONSTANT(sqrt2_v);
    FIXED_CONSTANT(sqrt3_v);


#undef FIXED_CONSTANT


}

#endif
