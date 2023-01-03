#ifndef RNG_HPP
#define RNG_HPP

#include <concepts>
#include <random>

#include <fxd/fxd.hpp>

#include "catch2/catch_amalgamated.hpp"



std::default_random_engine random_engine;


template<fxd::fixed_point Fxd>
struct RNG {

    fxd::uniform_real_distribution<Fxd> distribution;

    Fxd
    get()
        noexcept
    {
        return distribution(random_engine);
    }

};




#endif
