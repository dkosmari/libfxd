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
            auto r = fpToString(to_float(value), precision);
            if constexpr (std::unsigned_integral<typename Fxd::raw_type>)
                r += "_ufix<";
            else
                r += "_fix<";
            std::string result = r
                + std::to_string(Fxd::int_bits)
                + ","
                + std::to_string(Fxd::frac_bits)
                + ">  [ ";
            if constexpr (sizeof(typename Fxd::raw_type) == 1) {
                result += StringMaker<int>::convert(value.raw_value);
            } else {
                result += StringMaker<typename Fxd::raw_type>::convert(value.raw_value);
            }
            result += " ]";
            return result;
        }
    };


    template<typename... T>
    struct StringMaker<std::tuple<T...>> {

        template<std::size_t... Idx>
        static
        std::string
        conv_helper(const std::tuple<T...>& tup,
                    std::index_sequence<Idx...>)
        {
            return ((StringMaker<T>::convert(get<Idx>(tup)) + " , ") + ... + "");
        }


        static
        std::string
        convert(const std::tuple<T...>& tup)
        {
            using Seq = std::index_sequence_for<T...>;
            return "[ " + conv_helper(tup, Seq{}) + " ]";
        }
    };


    template<>
    struct StringMaker<std::strong_ordering> {
        static
        std::string
        convert(const std::strong_ordering& value)
        {
            if (value < 0)
                return "<";
            if (value > 0)
                return ">";
            return "==";
        }
    };

}


class Setup : public Catch::EventListenerBase {
public:
    using Catch::EventListenerBase::EventListenerBase;

    void
    testRunStarting(const Catch::TestRunInfo&)
        override
    {
        std::fesetround(FE_TOWARDZERO);
        random_engine.seed(Catch::getSeed());

        Catch::StringMaker<float>::precision = std::numeric_limits<float>::max_digits10;
        Catch::StringMaker<double>::precision = std::numeric_limits<double>::max_digits10;

    }
};

CATCH_REGISTER_LISTENER(Setup)

#if defined(__clang__) && __clang_major__ >= 12 && !defined(__FAST_MATH__)
#pragma STDC FENV_ACCESS ON
#endif

#endif
