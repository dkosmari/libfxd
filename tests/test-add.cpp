#include <fxd/fxd.hpp>

#include "catch2/catch_amalgamated.hpp"


TEST_CASE("basic", "[16.16]")
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
