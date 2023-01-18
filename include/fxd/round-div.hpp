#ifndef LIBFXD_ROUND_DIV_HPP
#define LIBFXD_ROUND_DIV_HPP

#include <csignal>

// #include <iostream>

#include "fixed.hpp"

#include "concepts.hpp"
#include "utils-div.hpp"
#include "utils-shift.hpp"
#include "utils.hpp"


namespace fxd::round {

    namespace zero {

        template<fixed_point Fxd>
        ALWAYS_INLINE
        constexpr
        Fxd
        div(Fxd a,
            Fxd b)
            noexcept
        {
            const auto r = utils::div::div<Fxd::frac_bits>(a.raw_value,
                                                           b.raw_value);
            if (!r)
                std::raise(SIGFPE);

            auto [c, expo, rem] = *r;

            const int offset = expo + Fxd::frac_bits;

            if (c < 0 && offset < 0)
                c += utils::shift::make_bias_for(-offset, c);

            const auto d = utils::shift::shl(c, offset);

            return Fxd::from_raw(d);
        }

    }

}


#endif
