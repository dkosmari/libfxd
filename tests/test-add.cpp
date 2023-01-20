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
    CHECK(a1 + a2 == a3);


    F b1 = -1;
    F b2 = -2;
    F b3 = -3;
    CHECK(b1 + b2 == b3);


    F c1 = 0.5;
    F c2 = 1.5;
    F c3 = 2;
    CHECK(c1 + c2 == c3);

}


TEMPLATE_LIST_TEST_CASE("random-sat",
                        "[saturation]",
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
        Fxd c = fxd::safe::saturate::add(a, b);
        auto fa = to_float(a);
        auto fb = to_float(b);
        auto fc = fa + fb;

        CAPTURE(a);
        CAPTURE(b);
        CAPTURE(c);
        CAPTURE(fc);

        if (fc < flo)
            REQUIRE(c == lo);
        else if (fhi < fc)
            REQUIRE(c == hi);
        else
            REQUIRE(c == Fxd{fc});

    }
}
