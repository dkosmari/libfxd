#include <cfenv>
#include <tuple>

#include "catch2/catch_amalgamated.hpp"

#include <fxd/fxd.hpp>

#include "printer.hpp"



TEST_CASE("basic_s16.16", "[s16.16]")
{
    using F = fxd::fixed<16, 16>;

    F a1 = 3;
    F a2 = 2;
    F a3 = 6;
    CHECK(a1 * a2 == a3);
}


TEST_CASE("round_s16.16", "[s16.16]")
{
    using F = fxd::fixed<16, 16>;

    {
        F a = F::from_raw(1);
        F b = 0.5;
        F c = 0;
        CHECK(a * b == c);
    }

    {
        F a = F::from_raw(-1);
        F b = 0.5;
        F c = 0;
        CHECK(a * b == c);
    }

    {
        F a = F::from_raw(2);
        F b = 0.5;
        F c = F::from_raw(1);
        CHECK(a * b == c);
    }

    {
        F a = F::from_raw(-2);
        F b = 0.5;
        F c = F::from_raw(-1);
        CHECK(a * b == c);
    }

    {
        F a = F::from_raw(3);
        F b = 0.5;
        F c = F::from_raw(1);
        CHECK(a * b == c);
    }

    {
        F a = F::from_raw(-3);
        F b = 0.5;
        F c = F::from_raw(-1);
        CHECK(a * b == c);
    }

    {
        F a = std::numeric_limits<F>::max();
        F b = std::numeric_limits<F>::min();
        F c = 0.5 - std::numeric_limits<F>::epsilon();
        CHECK(a * b == c);
    }

    {
        F a = std::numeric_limits<F>::lowest();
        F b = std::numeric_limits<F>::min();
        F c = -0.5;
        CHECK(a * b == c);
    }

    {
        F a = std::numeric_limits<F>::lowest() + std::numeric_limits<F>::epsilon();
        F b = std::numeric_limits<F>::min();
        F c = -0.5 + std::numeric_limits<F>::epsilon();
        CHECK(a * b == c);
    }

}


TEST_CASE("basic_s32.32", "[s32.32]")
{
    using F = fxd::fixed<32, 32>;

    {
        F a = 1;
        F b = 1;
        F c = 1;
        CHECK(a * b == c);
    }

    {
        F a = -1;
        F b = 1;
        F c = -1;
        CHECK(a * b == c);
    }

    {
        F a = -1;
        F b = -1;
        F c = 1;
        CHECK(a * b == c);
    }

    {
        F a = 2;
        F b = 3;
        F c = 6;
        CHECK(a * b == c);
    }

}


TEST_CASE("round_s32.32", "[s32.32]")
{
    using F = fxd::fixed<32, 32>;

    {
        F a = F::from_raw(-1);
        F b = 0.5;
        F c = 0;
        CHECK(a * b == c);
    }


    {
        F a = F::from_raw(-2);
        F b = 0.5;
        F c = F::from_raw(-1);
        F d = a * b;
        CHECK(d == c);
    }


    {
        F a = F::from_raw(-3);
        F b = 0.5;
        F c = F::from_raw(-1);
        F d = a * b;
        CHECK(d == c);
    }

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

    tuple<fxd::fixed<23, 20>,  double>,
    tuple<fxd::fixed<52, 1>,   double>,
    tuple<fxd::fixed<53, 0>,   double>,
    tuple<fxd::fixed<54, -1>,  double>,
    tuple<fxd::fixed<1, 52>,   double>,
    tuple<fxd::fixed<0, 53>,   double>,
    tuple<fxd::fixed<-1, 54>,  double>,

    tuple<fxd::ufixed<22, 20>, double>,
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




TEMPLATE_LIST_TEST_CASE("random",
                        "",
                        test_types)
{
    using Fxd = std::tuple_element_t<0, TestType>;
    using Flt = std::tuple_element_t<1, TestType>;

    constexpr auto lo = std::numeric_limits<Fxd>::lowest();
    constexpr auto hi = std::numeric_limits<Fxd>::max();

    constexpr Flt flo = static_cast<Flt>(lo);
    constexpr Flt fhi = static_cast<Flt>(hi);

    auto g = GENERATE(take(100000,
                           random(lo.raw_value,
                                  hi.raw_value)));

    Fxd a = Fxd::from_raw(g);
    Fxd b = Fxd::from_raw(g);
    Flt d = static_cast<Flt>(a) * static_cast<Flt>(b);
    if (flo <= d && d <= fhi) {
        Fxd c = a * b;
        REQUIRE(c == Fxd(d));
    }
}



class SetRound : public Catch::EventListenerBase {
public:
    using Catch::EventListenerBase::EventListenerBase;

    void testRunStarting(Catch::TestRunInfo const&) override
    {
        std::fesetround(FE_TOWARDZERO);
    }
};

CATCH_REGISTER_LISTENER(SetRound)
