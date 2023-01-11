#include <fxd/fxd.hpp>

#include "catch2/catch_amalgamated.hpp"

#include "printer.hpp"
#include "rng.hpp"
#include "setup.hpp"
#include "test-types.hpp"


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
        Fxd c = fxd::safe::saturate::plus(a, b);
        Flt d = static_cast<Flt>(a) + static_cast<Flt>(b);

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
