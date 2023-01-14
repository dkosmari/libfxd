#include <fxd/fxd.hpp>

#include "catch2/catch_amalgamated.hpp"

#include "printer.hpp"
#include "rng.hpp"
#include "setup.hpp"
#include "test-types.hpp"


TEST_CASE("basic", "[s16.16]")
{
    using Fxd = fxd::fixed<16, 16>;

    {
        Fxd a = 1;
        Fxd b = 2;
        Fxd c = -1;
        CHECK(a - b == c);
    }

    {
        Fxd a = 5;
        Fxd b = 5;
        Fxd c = 0;
        CHECK(a - b == c);
    }


    {
        Fxd a = 1.5;
        Fxd b = 2.25;
        Fxd c = -0.75;
        CHECK(a - b == c);
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
        Fxd c = sat::sub(a, b);
        CHECK(c == std::numeric_limits<Fxd>::max());
        CHECK_THROWS_AS(exc::sub(a, b), std::overflow_error);
    }

    {
        Fxd a = Fxd::from_raw(-1);
        Fxd b = std::numeric_limits<Fxd>::lowest();
        Fxd c = sat::sub(a, b);
        CHECK(c == std::numeric_limits<Fxd>::max());
        CHECK_NOTHROW(exc::sub(a, b));
    }

}


TEMPLATE_LIST_TEST_CASE("random-sat",
                        "[saturation]",
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
        Fxd c = fxd::safe::saturate::sub(a, b);
        Flt d = static_cast<Flt>(a) - static_cast<Flt>(b);

        CAPTURE(a);
        CAPTURE(b);
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
