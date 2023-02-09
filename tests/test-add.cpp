#include <stdexcept>

#include <fxd/fxd.hpp>

#include "rng.hpp"
#include "setup.hpp"
#include "test-types.hpp"


const int max_iterations = 1000;


TEMPLATE_LIST_TEST_CASE("random",
                        "",
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

    for (int i = 0; i < max_iterations; ++i) {

        Fxd a = rng.get();
        Fxd b = rng.get();
        Fxd sc = fxd::saturate::add(a, b);
        auto fa = to_float(a);
        auto fb = to_float(b);
        auto fc = fa + fb;

        CAPTURE(a);
        CAPTURE(b);
        CAPTURE(sc);
        CAPTURE(fc);

        if (fc < flo) {
            REQUIRE(sc == lo);
            REQUIRE_THROWS_AS(fxd::except::add(a, b), std::underflow_error);
        } else if (fhi < fc) {
            REQUIRE(sc == hi);
            REQUIRE_THROWS_AS(fxd::except::add(a, b), std::overflow_error);
        } else {
            Fxd c = a + b;
            REQUIRE(c == Fxd{fc});
            REQUIRE(sc == c);
            REQUIRE_NOTHROW(fxd::except::add(a, b) == c);
        }

    }
}




TEST_CASE("special-1", "[s16.16]")
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
