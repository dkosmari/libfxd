#include <cfenv>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <tuple>

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


TEST_CASE("mixed1")
{
    using A = fxd::fixed<4, 12>;
    using B = fxd::fixed<12, 4>;

    CHECK(A{2.5} == B{2.5});
    CHECK(A{2.5} <=> B{2.5} == 0);
}


namespace {
    using std::tuple;
    using fxd::fixed;
    using fxd::ufixed;

    using cmp_types = tuple<
        tuple<fixed<4, 4>, fixed<4, 4>>,
        tuple<fixed<4, 4>, ufixed<4, 4>>,
        tuple<ufixed<4, 4>, fixed<4, 4>>,
        tuple<ufixed<4, 4>, ufixed<4, 4>>,

        tuple<fixed<6, 2>, fixed<2, 6>>,
        tuple<fixed<6, 2>, ufixed<2, 6>>,
        tuple<ufixed<6, 2>, fixed<2, 6>>,
        tuple<ufixed<6, 2>, ufixed<2, 6>>,

        tuple<fixed<10, -2>, fixed<-2, 10>>,
        tuple<fixed<10, -2>, ufixed<-2, 10>>,
        tuple<ufixed<10, -2>, fixed<-2, 10>>,
        tuple<ufixed<10, -2>, ufixed<-2, 10>>
        >;
}



TEMPLATE_LIST_TEST_CASE("mixed-all",
                        "",
                        cmp_types)
{
    using A = std::tuple_element_t<0, TestType>;
    using B = std::tuple_element_t<1, TestType>;

    using W = fxd::fixed<32, 32>;

    using RawA = typename A::raw_type;
    using RawB = typename B::raw_type;

    using WideRawA = fxd::detail::double_width_t<RawA>;
    using WideRawB = fxd::detail::double_width_t<RawB>;

    using LimA = std::numeric_limits<A>;
    using LimB = std::numeric_limits<B>;

    for (WideRawA raw_a = LimA::lowest().raw_value;
         raw_a <= LimA::max().raw_value;
         ++raw_a) {

        const A a = A::from_raw(raw_a);
        const W wa = a;
        CAPTURE(a);
        CAPTURE(wa);

        for (WideRawB raw_b = LimB::lowest().raw_value;
             raw_b <= LimB::max().raw_value;
             ++raw_b) {

            const B b = B::from_raw(raw_b);
            const W wb = b;
            CAPTURE(b);
            CAPTURE(wb);

            CHECK((a <=> b) == (wa <=> wb));

        }

    }
}



TEST_CASE("special-1")
{
    using A = fxd::fixed<6, 2>;
    using B = fxd::fixed<2, 6>;

    A a = A::from_raw(-8);
    B b = B::from_raw(-127);

    CHECK(a < b);
}
