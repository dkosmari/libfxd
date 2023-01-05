#include <tuple>

#include <fxd/fxd.hpp>

#include "catch2/catch_amalgamated.hpp"

#include "printer.hpp"
#include "rng.hpp"
#include "setup.hpp"


TEST_CASE("basic", "[s16.16]")
{
    using F = fxd::fixed<16, 16>;

    F a1 = 1;
    F a2 = 2;
    F a3 = 3;
    REQUIRE(a1 + a2 == a3);


    F b1 = -1;
    F b2 = -2;
    F b3 = -3;
    REQUIRE(b1 + b2 == b3);


    F c1 = 0.5;
    F c2 = 1.5;
    F c3 = 2;
    REQUIRE(c1 + c2 == c3);

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
    // SHOW(lo);
    // SHOW(hi);

    const Flt flo = static_cast<Flt>(lo);
    const Flt fhi = static_cast<Flt>(hi);

    RNG<Fxd> rng;

    for (int i = 0; i < 10000; ++i) {

        Fxd a = rng.get();
        Fxd b = rng.get();
        Fxd c = fxd::safe::saturate::plus(a, b);
        Flt d = static_cast<Flt>(a) + static_cast<Flt>(b);

        // SHOW(a);
        // SHOW(b);
        // SHOW(c);

        if (d < flo)
            REQUIRE(c == lo);
        else if (d < fhi)
            REQUIRE(c == Fxd{d});
        else
            REQUIRE(c == hi);

    }
}
