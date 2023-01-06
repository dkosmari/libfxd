#ifndef SETUP_HPP
#define SETUP_HPP

#include <cfenv>
#include <iostream>

#include "rng.hpp"


class Setup : public Catch::EventListenerBase {
public:
    using Catch::EventListenerBase::EventListenerBase;

    void
    testRunStarting(Catch::TestRunInfo const&)
        override
    {
        std::cout << "Setting rounding mode toward zero...";
        if (std::fesetround(FE_TOWARDZERO))
            std::cout << "failed\n";
        else
            std::cout << "ok\n";
        random_engine.seed(Catch::getSeed());
    }
};

CATCH_REGISTER_LISTENER(Setup)


#endif
