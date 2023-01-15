#include <cstdlib>
#include <string>

#include "catch2/catch_amalgamated.hpp"

#include <fxd/fxd.hpp>

#include "printer.hpp"
#include "rng.hpp"
#include "setup.hpp"
#include "test-types.hpp"



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


TEST_CASE("special1")
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


TEST_CASE("special2")
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


TEST_CASE("special3")
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


TEST_CASE("special4")
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

    Fxd d = fxd::safe::saturate::div(a, b);
    CHECK(d == Fxd{ab});

    CHECK_NOTHROW(fxd::safe::except::div(a, b));
}


TEST_CASE("special5")
{
    using Fxd = fxd::fixed<1, 24>;

    Fxd a = Fxd::from_raw(-14324920);
    Fxd b = Fxd::from_raw(-98031);
    CAPTURE(a);
    CAPTURE(b);

    auto ab = to_float(a) / to_float(b);
    CAPTURE(ab);

    const auto hi = std::numeric_limits<Fxd>::max();
    Fxd d = fxd::safe::saturate::div(a, b);
    CHECK(d == hi);

    CHECK_THROWS_AS(fxd::safe::except::div(a, b), std::overflow_error);
}


TEST_CASE("special6")
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

    Fxd d = fxd::safe::saturate::div(a, b);
    CHECK(d == Fxd{ab});

    CHECK_NOTHROW(fxd::safe::except::div(a, b));
}



TEMPLATE_LIST_TEST_CASE("random-unsafe",
                        "[random][unsafe]",
                        test_types)
{
    using Fxd = TestType;

    constexpr Fxd lo = std::numeric_limits<Fxd>::lowest();
    constexpr Fxd hi = std::numeric_limits<Fxd>::max();

    const auto flo = to_float(lo);
    const auto fhi = to_float(hi);

    RNG<Fxd> rng;

    for (int i = 0; i < 10000; ++i) {
        Fxd a = rng.get();
        Fxd b = rng.get();
        CAPTURE(a);
        CAPTURE(b);

        auto ab = to_float(a) / to_float(b);
        CAPTURE(ab);

        if (b && flo <= ab && ab <= fhi) {
            Fxd c = a / b;
            REQUIRE(c == Fxd{ab});
        }
    }
}


TEMPLATE_LIST_TEST_CASE("random-safe",
                        "[safe][random]",
                        test_types)
{
    using Fxd = TestType;

    constexpr Fxd lo = std::numeric_limits<Fxd>::lowest();
    constexpr Fxd hi = std::numeric_limits<Fxd>::max();

    CAPTURE(lo);
    CAPTURE(hi);

    const auto flo = to_float(lo);
    const auto fhi = to_float(hi);

    CAPTURE(flo);
    CAPTURE(fhi);

    RNG<Fxd> rng;

    for (int i = 0; i < 10000; ++i) {
        Fxd a = rng.get();
        Fxd b = rng.get();

        if (!a && !b)
            continue;

        CAPTURE(a);
        CAPTURE(b);

        Fxd c = fxd::safe::saturate::div(a, b);

        auto ab = to_float(a) / to_float(b);
        CAPTURE(ab);

        if (ab < flo) {
            REQUIRE_THROWS_AS(fxd::safe::except::div(a, b), std::underflow_error);
            REQUIRE(c == lo);
        } else if (fhi < ab) {
            REQUIRE_THROWS_AS(fxd::safe::except::div(a, b), std::overflow_error);
            REQUIRE(c == hi);
        } else {
            REQUIRE_NOTHROW(fxd::safe::except::div(a, b));
            REQUIRE(c == Fxd{ab});
        }

    }
 }
