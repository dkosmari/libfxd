#ifndef SETUP_HPP
#define SETUP_HPP

#include <cfenv>
#include <concepts>
#include <iomanip>
#include <limits>
#include <ostream>
#include <sstream>
#include <string>

#include <fxd/fxd.hpp>

#include "catch2/catch_amalgamated.hpp"

#include "rng.hpp"


namespace {
    template<typename T>
    std::string
    fpToString(T value, int precision)
    {
        using std::isnan;
        if constexpr (std::floating_point<T>)
            if (isnan(value))
                return "nan";

        std::ostringstream out;
        out << std::setprecision(precision)
            << std::fixed
            << value;
        std::string d = out.str();
        std::size_t i = d.find_last_not_of('0');
        if (i != std::string::npos && i != d.size() - 1) {
            if (d[i] == '.')
                i++;
            d = d.substr(0, i + 1);
        }
        return d;
    }
}



namespace Catch {

    template<>
    struct StringMaker<long double> {
        static
        std::string
        convert(const long double& value)
        {
            const int precision = std::numeric_limits<long double>::max_digits10;
            return fpToString(value, precision) + "L";
        }
    };


    template<fxd::fixed_point Fxd>
    struct StringMaker<Fxd> {
        static
        std::string
        convert(Fxd value)
        {
            const int precision = std::numeric_limits<Fxd>::max_digits10;
            return fpToString(value, precision) + "_fix";
        }
    };
}


class Setup : public Catch::EventListenerBase {
public:
    using Catch::EventListenerBase::EventListenerBase;

    void
    testRunStarting(Catch::TestRunInfo const&)
        override
    {
        std::fesetround(FE_TOWARDZERO);
        random_engine.seed(Catch::getSeed());

        Catch::StringMaker<float>::precision = std::numeric_limits<float>::max_digits10;
        Catch::StringMaker<double>::precision = std::numeric_limits<double>::max_digits10;

    }
};

CATCH_REGISTER_LISTENER(Setup)


#endif
