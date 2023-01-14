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
    using Flt = long double;

    Fxd a = Fxd::from_raw(0x0db0a5a5d9d2167e);
    Fxd b = Fxd::from_raw(0x5f80ef4415b492ad);
    Fxd c = a / b;

    Flt fa = static_cast<Flt>(a);
    Flt fb = static_cast<Flt>(b);
    Flt fd = fa / fb;

    Fxd d = fd;

    CHECK(c == d);
}



TEMPLATE_LIST_TEST_CASE("random-basic",
                        "[random][raw]",
                        test_types)
{
    using Fxd = TestType;
    using Flt = Fxd::float_type;

    constexpr Fxd lo = std::numeric_limits<Fxd>::lowest();
    constexpr Fxd hi = std::numeric_limits<Fxd>::max();

    const Flt flo = static_cast<Flt>(lo);
    const Flt fhi = static_cast<Flt>(hi);

    RNG<Fxd> rng;

    for (int i = 0; i < 10000; ++i) {
        Fxd a = rng.get();
        Fxd b = rng.get();

        Flt fa = static_cast<Flt>(a);
        Flt fb = static_cast<Flt>(b);
        Flt fc = fa / fb;
        if (b && flo <= fc && fc <= fhi) {
            Fxd c = a / b;
            Fxd cc{fc};
            CHECK(c == cc);
        }
    }
}


TEMPLATE_LIST_TEST_CASE("random-sat",
                        "[saturation][random]",
                        test_types)
{
    using Fxd = TestType;
    using Flt = Fxd::float_type;

    constexpr Fxd lo = std::numeric_limits<Fxd>::lowest();
    constexpr Fxd hi = std::numeric_limits<Fxd>::max();

    CAPTURE(lo);
    CAPTURE(hi);

    const Flt flo = static_cast<Flt>(lo);
    const Flt fhi = static_cast<Flt>(hi);

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

        Flt fa = static_cast<Flt>(a);
        Flt fb = static_cast<Flt>(b);
        Flt d = fa / fb;

        CAPTURE(c);
        CAPTURE(d);

        if (d < flo)
            REQUIRE(c == lo);
        else if (d < fhi)
            REQUIRE(c == Fxd{d});
        else
            REQUIRE(c == hi);

    }
 }
