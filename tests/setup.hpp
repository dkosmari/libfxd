#ifndef SETUP_HPP
#define SETUP_HPP

#include <cfenv>

#include "random.hpp"


class Setup : public Catch::EventListenerBase {
public:
    using Catch::EventListenerBase::EventListenerBase;

    void
    testRunStarting(Catch::TestRunInfo const&)
        override
    {
        std::fesetround(FE_TOWARDZERO);
        random_engine.seed(Catch::getSeed());
    }
};

CATCH_REGISTER_LISTENER(Setup)


#endif
