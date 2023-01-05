#include <tuple>

#include <fxd/fxd.hpp>

#include "catch2/catch_amalgamated.hpp"

#include "printer.hpp"
#include "rng.hpp"
#include "setup.hpp"


TEST_CASE("basic", "[s16.16]")
{
    using Fxd = fxd::fixed<16, 16>;

    {
        Fxd a = 1;
        Fxd b = 2;
        Fxd c = -1;
        REQUIRE(a - b == c);
    }

    {
        Fxd a = 5;
        Fxd b = 5;
        Fxd c = 0;
        REQUIRE(a - b == c);
    }


    {
        Fxd a = 1.5;
        Fxd b = 2.25;
        Fxd c = -0.75;
        REQUIRE(a - b == c);
    }

}


TEST_CASE("extremes", "[s16.16][saturation][exception]")
{
    using Fxd = fxd::fixed<16, 16>;
    using sat = fxd::safe::saturate;
    using exc = fxd::safe::except;

    {
        Fxd a = 0;
        Fxd b = std::numeric_limits<Fxd>::lowest();
        Fxd c = sat::minus(a, b);
        REQUIRE(c == std::numeric_limits<Fxd>::max());
        REQUIRE_THROWS_AS(exc::minus(a, b), std::overflow_error);
    }

    {
        Fxd a = Fxd::from_raw(-1);
        Fxd b = std::numeric_limits<Fxd>::lowest();
        Fxd c = sat::minus(a, b);
        REQUIRE(c == std::numeric_limits<Fxd>::max());
        REQUIRE_NOTHROW(exc::minus(a, b));
    }

}



using std::tuple;

using test_types = tuple<
    fxd::fixed<13, 12>,
    fxd::fixed<1, 24>,
    fxd::fixed<0, 25>,
    fxd::fixed<-1, 26>,
    fxd::fixed<24, 1>,
    fxd::fixed<25, 0>,
    fxd::fixed<26, -1>,

    fxd::ufixed<12, 12>,
    fxd::ufixed<1, 23>,
    fxd::ufixed<0, 24>,
    fxd::ufixed<-1, 25>,
    fxd::ufixed<23, 1>,
    fxd::ufixed<24, 0>,
    fxd::ufixed<25, -1>,

    fxd::fixed<33, 20>,
    fxd::fixed<52, 1>,
    fxd::fixed<53, 0>,
    fxd::fixed<54, -1>,
    fxd::fixed<1, 52>,
    fxd::fixed<0, 53>,
    fxd::fixed<-1, 54>,

    fxd::ufixed<20, 32>,
    fxd::ufixed<51, 1>,
    fxd::ufixed<52, 0>,
    fxd::ufixed<53, -1>,
    fxd::ufixed<1, 51>,
    fxd::ufixed<0, 52>,
    fxd::ufixed<-1, 53>,

    fxd::fixed<32, 32>,
    fxd::fixed<63, 1>,
    fxd::fixed<64, 0>,
    fxd::fixed<65, -1>,
    fxd::fixed<1, 63>,
    fxd::fixed<0, 64>,
    fxd::fixed<-1, 65>,

    fxd::ufixed<32, 32>,
    fxd::ufixed<63, 1>,
    fxd::ufixed<64, 0>,
    fxd::ufixed<65, -1>,
    fxd::ufixed<1, 63>,
    fxd::ufixed<0, 64>,
    fxd::ufixed<-1, 65>
    >;


TEMPLATE_LIST_TEST_CASE("random-sat",
                        "[saturation]",
                        test_types)
{
    using Fxd = TestType;
    using Flt = Fxd::float_type;

    constexpr Fxd lo = std::numeric_limits<Fxd>::lowest();
    constexpr Fxd hi = std::numeric_limits<Fxd>::max();

    const Flt flo = static_cast<Flt>(lo);
    const Flt fhi = static_cast<Flt>(hi);

    RNG<Fxd> rng;

    for (int i = 0; i < 100000; ++i) {

        Fxd a = rng.get();
        Fxd b = rng.get();
        Fxd c = fxd::safe::saturate::minus(a, b);
        // SHOW(a);
        // SHOW(b);
        // SHOW(c);
        Flt d = static_cast<Flt>(a) - static_cast<Flt>(b);
        if (d < flo)
            REQUIRE(c == lo);
        else if (d < fhi)
            REQUIRE(c == Fxd{d});
        else
            REQUIRE(c == hi);

    }
}
