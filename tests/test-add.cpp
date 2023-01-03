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
    tuple<fxd::fixed<13, 12>,  float>,
    tuple<fxd::fixed<1, 24>,   float>,
    tuple<fxd::fixed<0, 25>,   float>,
    tuple<fxd::fixed<-1, 26>,  float>,
    tuple<fxd::fixed<24, 1>,   float>,
    tuple<fxd::fixed<25, 0>,   float>,
    tuple<fxd::fixed<26, -1>,  float>,

    tuple<fxd::ufixed<12, 12>, float>,
    tuple<fxd::ufixed<1, 23>,  float>,
    tuple<fxd::ufixed<0, 24>,  float>,
    tuple<fxd::ufixed<-1, 25>, float>,
    tuple<fxd::ufixed<23, 1>,  float>,
    tuple<fxd::ufixed<24, 0>,  float>,
    tuple<fxd::ufixed<25, -1>, float>,

    tuple<fxd::fixed<33, 20>,  double>,
    tuple<fxd::fixed<52, 1>,   double>,
    tuple<fxd::fixed<53, 0>,   double>,
    tuple<fxd::fixed<54, -1>,  double>,
    tuple<fxd::fixed<1, 52>,   double>,
    tuple<fxd::fixed<0, 53>,   double>,
    tuple<fxd::fixed<-1, 54>,  double>,

    tuple<fxd::ufixed<20, 32>, double>,
    tuple<fxd::ufixed<51, 1>,  double>,
    tuple<fxd::ufixed<52, 0>,  double>,
    tuple<fxd::ufixed<53, -1>, double>,
    tuple<fxd::ufixed<1, 51>,  double>,
    tuple<fxd::ufixed<0, 52>,  double>,
    tuple<fxd::ufixed<-1, 53>, double>,

    tuple<fxd::fixed<32, 32>,  long double>,
    tuple<fxd::fixed<63, 1>,   long double>,
    tuple<fxd::fixed<64, 0>,   long double>,
    tuple<fxd::fixed<65, -1>,  long double>,
    tuple<fxd::fixed<1, 63>,   long double>,
    tuple<fxd::fixed<0, 64>,   long double>,
    tuple<fxd::fixed<-1, 65>,  long double>,

    tuple<fxd::ufixed<32, 32>, long double>,
    tuple<fxd::ufixed<63, 1>,  long double>,
    tuple<fxd::ufixed<64, 0>,  long double>,
    tuple<fxd::ufixed<65, -1>, long double>,
    tuple<fxd::ufixed<1, 63>,  long double>,
    tuple<fxd::ufixed<0, 64>,  long double>,
    tuple<fxd::ufixed<-1, 65>, long double>
    >;


TEMPLATE_LIST_TEST_CASE("random-sat",
                        "[saturation]",
                        test_types)
{
    using Fxd = std::tuple_element_t<0, TestType>;
    using Flt = std::tuple_element_t<1, TestType>;

    constexpr auto lo = std::numeric_limits<Fxd>::lowest();
    constexpr auto hi = std::numeric_limits<Fxd>::max();
    // SHOW(lo);
    // SHOW(hi);

    constexpr Flt flo = static_cast<Flt>(lo);
    constexpr Flt fhi = static_cast<Flt>(hi);

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
