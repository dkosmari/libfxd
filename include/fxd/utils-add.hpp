#ifndef LIBFXD_UTILS_ADD_HPP
#define LIBFXD_UTILS_ADD_HPP

#include <concepts>
#include <tuple>

#include "utils-overflow.hpp"
#include "utils-tuple.hpp"


namespace fxd::utils::add {


    template<tuple::tuple_like Tup>
    constexpr
    Tup
    add(const Tup& a,
        const Tup& b,
        bool carry_in = false)
        noexcept
    {
        auto [sum, ovf] = overflow::add(tuple::first(a),
                                        tuple::first(b),
                                        carry_in);
        if constexpr (std::tuple_size_v<Tup> > 1)
            return std::tuple_cat(std::tuple{sum},
                                  add(tuple::tail(a),
                                      tuple::tail(b),
                                      ovf));
        else
            return { sum };
    }


    template<std::integral I>
    constexpr
    I
    add(I a,
        I b,
        bool carry_in = false)
    {
        return a + b + carry_in;
    }

}


#endif
