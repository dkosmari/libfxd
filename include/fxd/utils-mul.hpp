#ifndef LIBFXD_UTILS_MUL_HPP
#define LIBFXD_UTILS_MUL_HPP

#include <concepts>
#include <tuple> // tie()

#include "types.hpp"
#include "utils-overflow.hpp"


namespace fxd::utils::mul {


    // returns low, high parts of the full multiplication a * b
    // without using larger arithmetic than I
    template<std::integral I>
    constexpr
    std::pair<std::make_unsigned_t<I>, I>
    mul(I a,
        I b)
        noexcept
    {
        constexpr int k = type_width<I> / 2;
        using U = std::make_unsigned_t<I>;
        constexpr U mask = (U{1} << k) - 1;

        const U a0 = a & mask;
        const U a1 = a >> k; // note: intentional sign extension may happen

        const U b0 = b & mask;
        const U b1 = b >> k; // note: intentional sign extension may happen

        const I a0b1 = a0 * b1;
        const I a1b0 = a1 * b0;

        U c01 = a0 * b0;

        U carry1, carry2;
        std::tie(c01, carry1) = utils::overflow::add(c01, static_cast<U>(a0b1 & mask) << k);
        std::tie(c01, carry2) = utils::overflow::add(c01, static_cast<U>(a1b0 & mask) << k);

        I c23 = a1 * b1
            + (a0b1 >> k)
            + (a1b0 >> k)
            + carry1 + carry2;

        return {c01, c23};
    }

}


#endif
