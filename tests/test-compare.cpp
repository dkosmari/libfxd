#include <cfenv>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>

#include <fxd/fxd.hpp>

#include "catch2/catch_amalgamated.hpp"

#include "printer.hpp"
#include "setup.hpp"


TEST_CASE("basic", "[s16.16]")
{
    using Fxd = fxd::fixed<16, 16>;

    Fxd a = 5;
    Fxd b = 10;
    Fxd c = b - a;

    CHECK(a == c);
    CHECK(a != b);
    CHECK(a < b);
    CHECK(a <= b);
    CHECK(a <= c);
    CHECK(b > a);
    CHECK(b >= a);
    CHECK(a >= c);
}


TEST_CASE("integral", "[s16.16]")
{
    using Fxd1 = fxd::fixed<16, 16>;

    CHECK(Fxd1{5} == 5);
    CHECK(Fxd1{-3} == -3);
    CHECK(Fxd1{5} != 6);
    CHECK(Fxd1{-1} != 0u);
    CHECK(Fxd1{-1} != ~0u);

    using Fxd2 = fxd::fixed<1, 15>;

    CHECK(Fxd2{0} == 0);
    CHECK(Fxd2{0} != 4);
    CHECK(4 != Fxd2{0});
    CHECK(Fxd2{0} == 0);
    CHECK(0 == Fxd2{0});
    CHECK(Fxd2{-1} == -1);
    CHECK(-1 == Fxd2{-1});
    CHECK(Fxd2{-1} == -1);
    CHECK(-1 == Fxd2{-1});


    using Fxd3 = fxd::ufixed<1, 15>;

    CHECK(Fxd3{1} == 1);
    CHECK(1 == Fxd3{1});
    CHECK(Fxd3{1.5} != 1);
    CHECK(1 != Fxd3{1.5});
    CHECK(Fxd3{1.5} == 1.5);
    CHECK(1.5 == Fxd3{1.5});

    CHECK(Fxd3{0.5} != 0);
    CHECK(0 != Fxd3{0.5});

    CHECK(Fxd3{0.5} > 0);
    CHECK(0 < Fxd3{0.5});

    CHECK(Fxd3{0.5} >= 0);
    CHECK(0 <= Fxd3{0.5});


    using Fxd4 = fxd::fixed<6, 10>;

    CHECK(Fxd4{-0.5} < 0);
    CHECK(0 > Fxd4{-0.5});
    CHECK(Fxd4{-0.5} > -1);
    CHECK(-1 < Fxd4{-0.5});

    CHECK(Fxd4{-1.5} < 0);
    CHECK(0 > Fxd4{-1.5});
    CHECK(Fxd4{-1.5} < -1);
    CHECK(-1 > Fxd4{-1.5});
    CHECK(Fxd4{-1.5} > -2);
    CHECK(-2 < Fxd4{-1.5});



    using Fxd5 = fxd::fixed<18, -2>;

    CHECK(Fxd5{16} == 16);
    CHECK(16 == Fxd5{16});
    CHECK(Fxd5{16} != 20);
    CHECK(Fxd5{16} != 17);
    CHECK(17 != Fxd5{16});
    CHECK(Fxd5{16} < 17);
    CHECK(17 > Fxd5{16});
    CHECK(Fxd5{16} > 15);
    CHECK(15 < Fxd5{16});
    CHECK(Fxd5{16} < 20);
    CHECK(20 > Fxd5{16});
    CHECK(Fxd5{16} > 8);
    CHECK(8 < Fxd5{16});

    CHECK(Fxd5{-16} > -17);
    CHECK(-17 < Fxd5{-16});
    CHECK(Fxd5{-16} < -15);
    CHECK(-15 > Fxd5{-16});
    CHECK(Fxd5{-16} > -20);
    CHECK(-20 < Fxd5{-16});
    CHECK(Fxd5{-16} < -8);
    CHECK(-8 > Fxd5{-16});

}



TEST_CASE("float", "[s16.16]")
{
    using Fxd = fxd::fixed<10, 14>;
    CHECK(Fxd{1.5} == 1.5f);
    CHECK(1.5f == Fxd{1.5});
    CHECK(Fxd{1.5} < 2.5f);
    CHECK(2.5f > Fxd{1.5});
    CHECK(Fxd{1.5} > 0.5f);
    CHECK(0.5f < Fxd{1.5});
}
