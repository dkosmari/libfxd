#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <concepts>
#include <random>

#include <fxd/fxd.hpp>

#include "catch2/catch_amalgamated.hpp"



std::default_random_engine random_engine;


template<fxd::fixed_point Fxd>
struct RNG {

    std::uniform_int_distribution<typename Fxd::raw_type> distribution {
            std::numeric_limits<Fxd>::lowest().raw_value,
            std::numeric_limits<Fxd>::max().raw_value
    };

    Fxd
    get()
        noexcept
    {
        return Fxd::from_raw(distribution(random_engine));
    }

};




#endif
