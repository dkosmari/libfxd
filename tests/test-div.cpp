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

    Fxd d = fxd::safe::saturate::div(a, b);
    CHECK(d == Fxd{ab});

    CHECK_NOTHROW(fxd::safe::except::div(a, b));
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
    Fxd d = fxd::safe::saturate::div(a, b);
    CHECK(d == hi);

    CHECK_THROWS_AS(fxd::safe::except::div(a, b), std::overflow_error);
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

    Fxd d = fxd::safe::saturate::div(a, b);
    CHECK(d == Fxd{ab});

    CHECK_NOTHROW(fxd::safe::except::div(a, b));
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

    Fxd d = fxd::safe::saturate::div(a, b);
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

    Fxd d = fxd::safe::saturate::div(a, b);
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

    Fxd d = fxd::safe::saturate::div(a, b);
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

    Fxd d = fxd::safe::saturate::div(a, b);
    CHECK(d == hi);

    CHECK_THROWS_AS(fxd::safe::except::div(a, b), std::overflow_error);
}


TEMPLATE_LIST_TEST_CASE("random-unsafe",
                        "[random][unsafe][zero]",
                        test_types)
{
    using Fxd = TestType;

    constexpr Fxd lo = std::numeric_limits<Fxd>::lowest();
    constexpr Fxd hi = std::numeric_limits<Fxd>::max();

    CAPTURE(lo);
    CAPTURE(hi);

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



TEMPLATE_LIST_TEST_CASE("random-unsafe-up",
                        "[random][unsafe][up]",
                        test_types)
{
    using Fxd = TestType;

    constexpr Fxd lo = std::numeric_limits<Fxd>::lowest();
    constexpr Fxd hi = std::numeric_limits<Fxd>::max();

    CAPTURE(lo);
    CAPTURE(hi);

    const auto flo = to_float(lo);
    const auto fhi = to_float(hi);

    RNG<Fxd> rng;
    fxd::utils::round_up guard;

    for (int i = 0; i < 10000; ++i) {
        Fxd a = rng.get();
        Fxd b = rng.get();
        CAPTURE(a);
        CAPTURE(b);

        auto fc = to_float(a) / to_float(b);
        CAPTURE(fc);

        if (b && flo <= fc && fc <= fhi) {
            Fxd c = fxd::round::up::div(a, b);
            REQUIRE(c == Fxd{fc});
        }
    }
}



TEMPLATE_LIST_TEST_CASE("random-unsafe-down",
                        "[random][unsafe][down]",
                        test_types)
{
    using Fxd = TestType;

    constexpr Fxd lo = std::numeric_limits<Fxd>::lowest();
    constexpr Fxd hi = std::numeric_limits<Fxd>::max();

    CAPTURE(lo);
    CAPTURE(hi);

    const auto flo = to_float(lo);
    const auto fhi = to_float(hi);

    RNG<Fxd> rng;
    fxd::utils::round_down guard;

    for (int i = 0; i < 10000; ++i) {
        Fxd a = rng.get();
        Fxd b = rng.get();
        CAPTURE(a);
        CAPTURE(b);

        auto fc = to_float(a) / to_float(b);
        CAPTURE(fc);

        if (b && flo <= fc && fc <= fhi) {
            Fxd c = fxd::round::down::div(a, b);
            REQUIRE(c == Fxd{fc});
        }
    }
}
