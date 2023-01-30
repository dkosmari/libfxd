#include <cstdlib>
#include <string>

#include "catch2/catch_amalgamated.hpp"

#include <fxd/fxd.hpp>

#include "printer.hpp"
#include "rng.hpp"
#include "rounder.hpp"
#include "setup.hpp"
#include "test-types.hpp"


const int max_iterations = 1000;



TEST_CASE("basic", "[s16.16]")
{

    using F = fxd::fixed<16, 16>;

    {
        F a = 0;
        F b = 1;
        F c = 0;
        CHECK(a / b == c);
    }

    {
        F a = 1;
        F b = 1;
        F c = 1;
        CHECK(a / b == c);
    }

    {
        F a = -1;
        F b = 1;
        F c = -1;
        CHECK(a / b == c);
    }

    {
        F a = -1;
        F b = -1;
        F c = 1;
        CHECK(a / b == c);
    }

    {
        F a = 1;
        F b = 0.5L;
        F c = 2;
        CHECK(a / b == c);
    }

    {
        F a = std::numeric_limits<F>::max();
        F b = std::numeric_limits<F>::max() - 0.5;
        F c = 1 + std::numeric_limits<F>::epsilon();
        F d = a / b;
        CHECK(c == d);
    }

    {
        F a = -std::numeric_limits<F>::max();
        F b = std::numeric_limits<F>::max() - 0.5;
        F c = -(1 + std::numeric_limits<F>::epsilon());
        F d = a / b;
        CHECK(c == d);
    }

}


TEMPLATE_LIST_TEST_CASE("random-zero",
                        "[random][zero]",
                        test_types)
{
    using Fxd = TestType;

    constexpr Fxd lo = std::numeric_limits<Fxd>::lowest();
    constexpr Fxd hi = std::numeric_limits<Fxd>::max();
    constexpr Fxd ep = std::numeric_limits<Fxd>::epsilon();
    CAPTURE(lo);
    CAPTURE(hi);
    CAPTURE(ep);

    const auto flo = to_float(lo);
    const auto fhi = to_float(hi);

    RNG<Fxd> rng;

    for (int i = 0; i < max_iterations; ++i) {
        Fxd a = rng.get();
        Fxd b = rng.get();
        CAPTURE(a);
        CAPTURE(b);
        Fxd sc = fxd::saturate::div(a, b);

        auto fa = to_float(a);
        auto fb = to_float(b);
        auto fc =  fa / fb;
        CAPTURE(fa);
        CAPTURE(fb);
        CAPTURE(fc);

        if (fc < flo) {
            REQUIRE(sc == lo);
            REQUIRE_THROWS_AS(fxd::except::div(a, b), std::underflow_error);
        } else if (fc > fhi) {
            REQUIRE(sc == hi);
            REQUIRE_THROWS_AS(fxd::except::div(a, b), std::overflow_error);
        } else {
            Fxd c = a / b;
            REQUIRE(c == Fxd{fc});
            REQUIRE(c == sc);
            REQUIRE_NOTHROW(c == fxd::except::div(a, b));
        }
    }
}


TEMPLATE_LIST_TEST_CASE("random-up",
                        "[random][up]",
                        test_types)
{
    round_up guard;

    using Fxd = TestType;

    constexpr Fxd lo = std::numeric_limits<Fxd>::lowest();
    constexpr Fxd hi = std::numeric_limits<Fxd>::max();
    constexpr Fxd ep = std::numeric_limits<Fxd>::epsilon();
    CAPTURE(lo);
    CAPTURE(hi);
    CAPTURE(ep);

    const auto flo = to_float(lo);
    const auto fhi = to_float(hi);

    RNG<Fxd> rng;

    for (int i = 0; i < max_iterations; ++i) {
        Fxd a = rng.get();
        Fxd b = rng.get();
        CAPTURE(a);
        CAPTURE(b);
        Fxd sc = fxd::saturate::round::up::div(a, b);

        auto fa = to_float(a);
        auto fb = to_float(b);
        auto fc =  fa / fb;
        CAPTURE(fa);
        CAPTURE(fb);
        CAPTURE(fc);

        if (fc < flo) {
            REQUIRE(sc == lo);
            REQUIRE_THROWS_AS(fxd::except::round::up::div(a, b), std::underflow_error);
        } else if (fc > fhi) {
            REQUIRE(sc == hi);
            REQUIRE_THROWS_AS(fxd::except::round::up::div(a, b), std::overflow_error);
        } else {
            Fxd c = fxd::round::up::div(a, b);
            REQUIRE(c == Fxd{fc});
            REQUIRE(c == sc);
            REQUIRE_NOTHROW(c == fxd::except::round::up::div(a, b));
        }
    }
}


TEMPLATE_LIST_TEST_CASE("random-down",
                        "[random][down]",
                        test_types)
{
    round_down guard;

    using Fxd = TestType;

    constexpr Fxd lo = std::numeric_limits<Fxd>::lowest();
    constexpr Fxd hi = std::numeric_limits<Fxd>::max();
    constexpr Fxd ep = std::numeric_limits<Fxd>::epsilon();
    CAPTURE(lo);
    CAPTURE(hi);
    CAPTURE(ep);

    const auto flo = to_float(lo);
    const auto fhi = to_float(hi);

    RNG<Fxd> rng;

    for (int i = 0; i < max_iterations; ++i) {
        Fxd a = rng.get();
        Fxd b = rng.get();
        CAPTURE(a);
        CAPTURE(b);
        Fxd sc = fxd::saturate::round::down::div(a, b);

        auto fa = to_float(a);
        auto fb = to_float(b);
        auto fc =  fa / fb;
        CAPTURE(fa);
        CAPTURE(fb);
        CAPTURE(fc);

        if (fc < flo) {
            REQUIRE(sc == lo);
            REQUIRE_THROWS_AS(fxd::except::round::down::div(a, b), std::underflow_error);
        } else if (fc > fhi) {
            REQUIRE(sc == hi);
            REQUIRE_THROWS_AS(fxd::except::round::down::div(a, b), std::overflow_error);
        } else {
            Fxd c = fxd::round::down::div(a, b);
            REQUIRE(c == Fxd{fc});
            REQUIRE(c == sc);
            REQUIRE_NOTHROW(c == fxd::except::round::down::div(a, b));
        }
    }
}



TEST_CASE("special-1")
{
    using Fxd = fxd::fixed<-1, 65>;

    Fxd a = Fxd::from_raw(0x0db0a5a5d9d2167e);
    Fxd b = Fxd::from_raw(0x5f80ef4415b492ad);
    CAPTURE(a);
    CAPTURE(b);

    Fxd c = a / b;

    auto ab = to_float(a) / to_float(b);
    CAPTURE(ab);

    CHECK(c == Fxd{ab});
}


TEST_CASE("special-2")
{
    using Fxd = fxd::fixed<13, 12>;

    Fxd a = Fxd::from_raw(0x5fcc11);
    Fxd b = Fxd::from_raw(0x181f0);
    CAPTURE(a);
    CAPTURE(b);

    Fxd c = a / b;

    auto ab = to_float(a) / to_float(b);
    CAPTURE(ab);

    CHECK(c == Fxd{ab});
}


TEST_CASE("special-3")
{
    using Fxd = fxd::fixed<13, 12>;

    Fxd a = Fxd::from_raw(-9339753);
    Fxd b = Fxd::from_raw(44650);
    CAPTURE(a);
    CAPTURE(b);

    Fxd c = a / b;

    auto ab = to_float(a) / to_float(b);
    CAPTURE(ab);

    CHECK(c == Fxd{ab});
}


TEST_CASE("special-4")
{
    using Fxd = fxd::fixed<13, 12>;

    Fxd a = Fxd::from_raw(-5980327);
    Fxd b = Fxd::from_raw(178314);
    CAPTURE(a);
    CAPTURE(b);

    Fxd c = a / b;

    auto ab = to_float(a) / to_float(b);
    CAPTURE(ab);

    CHECK(c == Fxd{ab});

    Fxd d = fxd::saturate::div(a, b);
    CHECK(d == Fxd{ab});

    CHECK_NOTHROW(fxd::except::div(a, b));
}


TEST_CASE("special-5")
{
    using Fxd = fxd::fixed<1, 24>;

    Fxd a = Fxd::from_raw(-14324920);
    Fxd b = Fxd::from_raw(-98031);
    CAPTURE(a);
    CAPTURE(b);

    auto ab = to_float(a) / to_float(b);
    CAPTURE(ab);

    const auto hi = std::numeric_limits<Fxd>::max();
    Fxd d = fxd::saturate::div(a, b);
    CHECK(d == hi);

    CHECK_THROWS_AS(fxd::except::div(a, b), std::overflow_error);
}


TEST_CASE("special-6")
{
    using Fxd = fxd::fixed<24, 1>;

    Fxd a = Fxd::from_raw(2908892);
    Fxd b = Fxd::from_raw(-16596005);
    CAPTURE(a);
    CAPTURE(b);

    auto ab = to_float(a) / to_float(b);
    CAPTURE(ab);

    Fxd c = a / b;
    CHECK(c == Fxd{ab});

    Fxd d = fxd::saturate::div(a, b);
    CHECK(d == Fxd{ab});

    CHECK_NOTHROW(fxd::except::div(a, b));
}


TEST_CASE("special-7")
{
    using Fxd = fxd::fixed<13, 12>;

    Fxd a = Fxd::from_raw(1138);
    Fxd b = Fxd::from_raw(-9258039);
    CAPTURE(a);
    CAPTURE(b);

    auto ab = to_float(a) / to_float(b);
    CAPTURE(ab);

    Fxd c = a / b;
    CHECK(c == Fxd{ab});

    Fxd d = fxd::saturate::div(a, b);
    CHECK(d == Fxd{ab});
}


TEST_CASE("special-8")
{
    using Fxd = fxd::fixed<1, 63>;

    Fxd a = Fxd::from_raw(-6572227113762475346LL);
    Fxd b = Fxd::from_raw(8822511651387188039LL);
    CAPTURE(a);
    CAPTURE(b);

    auto ab = to_float(a) / to_float(b);
    CAPTURE(ab);

    Fxd c = a / b;
    CHECK(c == Fxd{ab});

    Fxd d = fxd::saturate::div(a, b);
    CHECK(d == Fxd{ab});
}


TEST_CASE("special-9")
{
    using Fxd = fxd::ufixed<1, 63>;

    Fxd a = Fxd::from_raw(5592189842187950237ULL);
    Fxd b = Fxd::from_raw(3706430751268532643ULL);
    CAPTURE(a);
    CAPTURE(b);

    auto ab = to_float(a) / to_float(b);
    CAPTURE(ab);

    Fxd c = a / b;
    CHECK(c == Fxd{ab});

    Fxd d = fxd::saturate::div(a, b);
    CHECK(d == Fxd{ab});
}


TEST_CASE("special-10")
{
    using Fxd = fxd::fixed<1, 24>;

    Fxd a = Fxd::from_raw(-14324920);
    Fxd b = Fxd::from_raw(-98031);
    CAPTURE(a);
    CAPTURE(b);
    Fxd lo = std::numeric_limits<Fxd>::lowest();
    Fxd hi = std::numeric_limits<Fxd>::max();
    CAPTURE(lo);
    CAPTURE(hi);

    auto ab = to_float(a) / to_float(b);
    CAPTURE(ab);

    Fxd d = fxd::saturate::div(a, b);
    CHECK(d == hi);

    CHECK_THROWS_AS(fxd::except::div(a, b), std::overflow_error);
}


TEST_CASE("special-11", "[up]")
{
    using Fxd = fxd::fixed<24, 1>;

    round_up guard;

    Fxd a = Fxd::from_raw(3977664);
    Fxd b = Fxd::from_raw(-8733244);
    CAPTURE(a);
    CAPTURE(b);

    auto fa = to_float(a);
    auto fb = to_float(b);
    auto fc = fa / fb;
    CAPTURE(fa);
    CAPTURE(fb);
    CAPTURE(fc);

    Fxd c = fxd::round::up::div(a, b);
    CHECK(c == Fxd{fc});
}


TEST_CASE("special-12", "[up]")
{
    using Fxd = fxd::fixed<13, 12>;

    round_up guard;

    Fxd a = Fxd::from_raw(8235717);
    Fxd b = Fxd::from_raw(12116731);
    CAPTURE(a);
    CAPTURE(b);

    auto fa = to_float(a);
    auto fb = to_float(b);
    auto fc = fa / fb;
    CAPTURE(fa);
    CAPTURE(fb);
    CAPTURE(fc);

    Fxd c = fxd::round::up::div(a, b);
    CHECK(c == Fxd{fc});
}


TEST_CASE("special-13", "[up]")
{
    using Fxd = fxd::fixed<13, 12>;

    round_up guard;

    Fxd a = Fxd::from_raw(1605);
    Fxd b = Fxd::from_raw(16714058);
    CAPTURE(a);
    CAPTURE(b);

    auto fa = to_float(a);
    auto fb = to_float(b);
    auto fc = fa / fb;
    CAPTURE(fa);
    CAPTURE(fb);
    CAPTURE(fc);

    Fxd c = fxd::round::up::div(a, b);
    CHECK(c == Fxd{fc});
}


TEST_CASE("special-14", "[up]")
{
    using Fxd = fxd::fixed<13, 12>;

    round_up guard;

    Fxd a = Fxd::from_raw(-407);
    Fxd b = Fxd::from_raw(4714082);
    CAPTURE(a);
    CAPTURE(b);

    auto fa = to_float(a);
    auto fb = to_float(b);
    auto fc = fa / fb;
    CAPTURE(fa);
    CAPTURE(fb);
    CAPTURE(fc);

    Fxd c = fxd::round::up::div(a, b);
    CHECK(c == Fxd{fc});
}


TEST_CASE("special-15", "[up]")
{
    using Fxd = fxd::fixed<26, -1>;

    round_up guard;

    Fxd a = Fxd::from_raw(11126233);
    Fxd b = Fxd::from_raw(-3231766);
    CAPTURE(a);
    CAPTURE(b);

    auto fa = to_float(a);
    auto fb = to_float(b);
    auto fc = fa / fb;
    CAPTURE(fa);
    CAPTURE(fb);
    CAPTURE(fc);

    Fxd c = fxd::round::up::div(a, b);
    CHECK(c == Fxd{fc});

}


TEST_CASE("special-16", "[up]")
{
    using Fxd = fxd::ufixed<-10, 34>;

    Fxd a = Fxd::from_raw(14022498);
    Fxd b = Fxd::from_raw(5098819);
    CAPTURE(a);
    CAPTURE(b);
    Fxd hi = std::numeric_limits<Fxd>::max();

    Fxd c = fxd::saturate::round::up::div(a, b);
    CHECK(c == hi);

    CHECK(fxd::saturate::div(a, b) == hi);
}


TEST_CASE("special-17", "[zero]")
{
    using Fxd = fxd::fixed<1, 53>;
    Fxd a = Fxd::from_raw(-8554731877316490LL); // -0.94976602996915571_fix<1,53>
    Fxd b = Fxd::from_raw(-5615165162258LL); // -0.00062340856502118_fix<1,53>
    CAPTURE(a);
    CAPTURE(b);
    Fxd hi = std::numeric_limits<Fxd>::max();
    Fxd sc = fxd::saturate::div(a, b);
    CHECK(sc == hi);
}


TEST_CASE("special-18", "[up]")
{
    /*
-------------------------------------------------------------------------------
random-up - test_types - 12
-------------------------------------------------------------------------------
test-div.cpp:128
...............................................................................

test-div.cpp:168: FAILED:
  REQUIRE( c == Fxd{fc} )
with expansion:
  0.000360095874_ufix<-10,34>  [ 6186400 (0x5e65a0) ]
  ==
  0.000360095001_ufix<-10,34>  [ 6186385 (0x5e6591) ]
with messages:
  a := 0.00000017602_ufix<-10,34>  [ 3024 (0xbd0) ]
  b := 0.000488815364_ufix<-10,34>  [ 8397784 (0x8023d8) ]
  fa := 0.000000177f
  fb := 0.000488816f
  fc := 0.000360096f
     */
    round_up guard;

    using Fxd = fxd::ufixed<-10, 34>;
    Fxd a = Fxd::from_raw(3024);
    Fxd b = Fxd::from_raw(8397784);
    Fxd c = fxd::round::up::div(a, b);
    CAPTURE(a);
    CAPTURE(b);
    CAPTURE(c);
    auto fa = to_float(a);
    auto fb = to_float(b);
    auto fc = fa / fb;
    CAPTURE(fa);
    CAPTURE(fb);
    CAPTURE(fc);
    CHECK(c == Fxd{fc});
}
