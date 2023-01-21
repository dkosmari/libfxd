#ifndef LIBFXD_HASH_HPP
#define LIBFXD_HASH_HPP

#include <functional>

#include "concepts.hpp"


namespace std {

    template<fxd::fixed_point Fxd>
    struct hash<Fxd> {

        constexpr
        size_t
        operator ()(Fxd f)
            const noexcept
        {
            using H = hash<typename Fxd::raw_type>;
            return H{}(f.raw_value);
        }

    };

}


#endif
