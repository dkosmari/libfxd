#include <cstdlib>
#include <cxxabi.h>
#include <string>
#include <tuple>

#include <fxd/fxd.hpp>

#include "catch2/catch_amalgamated.hpp"

#include "printer.hpp"
#include "rng.hpp"
#include "setup.hpp"



template<typename T>
std::string
type()
{
    char* s = abi::__cxa_demangle(typeid(T).name(), 0, 0, 0);
    std::string result = s;
    std::free(s);
    return result;
}


TEST_CASE("basic_s16.16", "[s16.16]")
{
    using F = fxd::fixed<16, 16>;

    F a1 = 3;
    F a2 = 2;
    F a3 = 6;
    CHECK(a1 * a2 == a3);
}


TEST_CASE("round_s16.16", "[s16.16]")
{
    using F = fxd::fixed<16, 16>;

    {
        F a = F::from_raw(1);
        F b = 0.5;
        F c = 0;
        CHECK(a * b == c);
    }

    {
        F a = F::from_raw(-1);
        F b = 0.5;
        F c = 0;
        CHECK(a * b == c);
    }

    {
        F a = F::from_raw(2);
        F b = 0.5;
        F c = F::from_raw(1);
        CHECK(a * b == c);
    }

    {
        F a = F::from_raw(-2);
        F b = 0.5;
        F c = F::from_raw(-1);
        CHECK(a * b == c);
    }

    {
        F a = F::from_raw(3);
        F b = 0.5;
        F c = F::from_raw(1);
        CHECK(a * b == c);
    }

    {
        F a = F::from_raw(-3);
        F b = 0.5;
        F c = F::from_raw(-1);
        CHECK(a * b == c);
    }

    {
        F a = std::numeric_limits<F>::max();
        F b = std::numeric_limits<F>::min();
        F c = 0.5 - std::numeric_limits<F>::epsilon();
        CHECK(a * b == c);
    }

    {
        F a = std::numeric_limits<F>::lowest();
        F b = std::numeric_limits<F>::min();
        F c = -0.5;
        CHECK(a * b == c);
    }

    {
        F a = std::numeric_limits<F>::lowest() + std::numeric_limits<F>::epsilon();
        F b = std::numeric_limits<F>::min();
        F c = -0.5 + std::numeric_limits<F>::epsilon();
        CHECK(a * b == c);
    }

}


TEST_CASE("basic_s32.32", "[s32.32]")
{
    using F = fxd::fixed<32, 32>;

    {
        F a = 1;
        F b = 1;
        F c = 1;
        CHECK(a * b == c);
    }

    {
        F a = -1;
        F b = 1;
        F c = -1;
        CHECK(a * b == c);
    }

    {
        F a = -1;
        F b = -1;
        F c = 1;
        CHECK(a * b == c);
    }

    {
        F a = 2;
        F b = 3;
        F c = 6;
        CHECK(a * b == c);
    }

}


TEST_CASE("round_s32.32", "[s32.32]")
{
    using F = fxd::fixed<32, 32>;

    {
        F a = F::from_raw(-1);
        F b = 0.5;
        F c = 0;
        CHECK(a * b == c);
    }


    {
        F a = F::from_raw(-2);
        F b = 0.5;
        F c = F::from_raw(-1);
        F d = a * b;
        CHECK(d == c);
    }


    {
        F a = F::from_raw(-3);
        F b = 0.5;
        F c = F::from_raw(-1);
        F d = a * b;
        CHECK(d == c);
    }

}


TEST_CASE("special1")
{
    using F = fxd::fixed<33, 20>;

    F a = F::from_raw(0xfff99a67f370c65a);
    F b = F::from_raw(0x0002bd0e4b41fb2d);
    F c = fxd::safe::saturate::multiplies(a, b);
    F d = std::numeric_limits<F>::lowest();

    // SHOW(a);
    // SHOW(b);
    // SHOW(c);
    // SHOW(d);

    REQUIRE(c == d);
}


TEST_CASE("special2")
{
    using std::cout;
    using std::endl;

    using I = std::int64_t;

    I a = 0xfff99a67f370c65a;
    I b = 0x0002bd0e4b41fb2d;
    auto c = fxd::utils::full_mult(a, b);
    REQUIRE(c.first == 0x814a34a018271bd2ULL);
    REQUIRE(c.second == static_cast<I>(0xffffffee7b7335e1LL));
}


TEST_CASE("special3")
{
    using F = fxd::fixed<-1, 65>;

    F a = F::from_raw(0x679d16dc561eef4d); // 0.202370371243032804
    F b = F::from_raw(0xabafbb5b0dac6969); // -0.164674897322732484
    F c = a * b;
    // SHOW(a);
    // SHOW(b);
    // SHOW(c);
    long double d = static_cast<long double>(a) * static_cast<long double>(b);
    REQUIRE(c == F{d});
}



using std::tuple;

using test_types = tuple<
    tuple<fxd::fixed<13, 12>,  float>,
    tuple<fxd::fixed<1, 24>,   float>,
    tuple<fxd::fixed<0, 25>,   float>,
    tuple<fxd::fixed<-1, 26>,  float>,
    tuple<fxd::fixed<24, 1>,   float>,
    tuple<fxd::fixed<25, 0>,   float>,
    tuple<fxd::fixed<26, -1>,  float>,

    tuple<fxd::ufixed<12, 12>, float>,
    tuple<fxd::ufixed<1, 23>,  float>,
    tuple<fxd::ufixed<0, 24>,  float>,
    tuple<fxd::ufixed<-1, 25>, float>,
    tuple<fxd::ufixed<23, 1>,  float>,
    tuple<fxd::ufixed<24, 0>,  float>,
    tuple<fxd::ufixed<25, -1>, float>,

    tuple<fxd::fixed<33, 20>,  double>,
    tuple<fxd::fixed<52, 1>,   double>,
    tuple<fxd::fixed<53, 0>,   double>,
    tuple<fxd::fixed<54, -1>,  double>,
    tuple<fxd::fixed<1, 52>,   double>,
    tuple<fxd::fixed<0, 53>,   double>,
    tuple<fxd::fixed<-1, 54>,  double>,

    tuple<fxd::ufixed<20, 32>, double>,
    tuple<fxd::ufixed<51, 1>,  double>,
    tuple<fxd::ufixed<52, 0>,  double>,
    tuple<fxd::ufixed<53, -1>, double>,
    tuple<fxd::ufixed<1, 51>,  double>,
    tuple<fxd::ufixed<0, 52>,  double>,
    tuple<fxd::ufixed<-1, 53>, double>,

    tuple<fxd::fixed<32, 32>,  long double>,
    tuple<fxd::fixed<63, 1>,   long double>,
    tuple<fxd::fixed<64, 0>,   long double>,
    tuple<fxd::fixed<65, -1>,  long double>,
    tuple<fxd::fixed<1, 63>,   long double>,
    tuple<fxd::fixed<0, 64>,   long double>,
    tuple<fxd::fixed<-1, 65>,  long double>,

    tuple<fxd::ufixed<32, 32>, long double>,
    tuple<fxd::ufixed<63, 1>,  long double>,
    tuple<fxd::ufixed<64, 0>,  long double>,
    tuple<fxd::ufixed<65, -1>, long double>,
    tuple<fxd::ufixed<1, 63>,  long double>,
    tuple<fxd::ufixed<0, 64>,  long double>,
    tuple<fxd::ufixed<-1, 65>, long double>
    >;


TEMPLATE_LIST_TEST_CASE("random-basic",
                        "",
                        test_types)
{
    using Fxd = std::tuple_element_t<0, TestType>;
    using Flt = std::tuple_element_t<1, TestType>;

    constexpr auto lo = std::numeric_limits<Fxd>::lowest();
    constexpr auto hi = std::numeric_limits<Fxd>::max();

    constexpr Flt flo = static_cast<Flt>(lo);
    constexpr Flt fhi = static_cast<Flt>(hi);

    RNG<Fxd> rng;

    for (int i = 0; i < 10000; ++i) {

        Fxd a = rng.get();
        Fxd b = rng.get();

        Flt d = static_cast<Flt>(a) * static_cast<Flt>(b);
        if (flo <= d && d <= fhi) {
            Fxd c = a * b;
            // std::cout << type<Fxd>() << '\n';
            // SHOW(lo);
            // SHOW(hi);
            // SHOW(a);
            // SHOW(b);
            // SHOW(c);
            // std::cout << "d = "
            //           << std::fixed
            //           << std::setprecision(17)
            //           << d << '\n';

            REQUIRE(c == Fxd{d});
        }

    }
}


TEMPLATE_LIST_TEST_CASE("random-sat",
                        "[saturation]",
                        test_types)
{
    using Fxd = std::tuple_element_t<0, TestType>;
    using Flt = std::tuple_element_t<1, TestType>;

    constexpr auto lo = std::numeric_limits<Fxd>::lowest();
    constexpr auto hi = std::numeric_limits<Fxd>::max();

    constexpr Flt flo = static_cast<Flt>(lo);
    constexpr Flt fhi = static_cast<Flt>(hi);

    RNG<Fxd> rng;

    for (int i = 0; i < 10000; ++i) {

        Fxd a = rng.get();
        Fxd b = rng.get();
        Fxd c = fxd::safe::saturate::multiplies(a, b);
        Flt d = static_cast<Flt>(a) * static_cast<Flt>(b);

        // SHOW(lo);
        // SHOW(hi);
        // SHOW(a);
        // SHOW(b);
        // SHOW(c);
        // std::cout << "d = "
        //           << std::fixed
        //           << std::setprecision(17)
        //           << d << '\n';

        if (d < flo)
            REQUIRE(c == lo);
        else if (d < fhi)
            REQUIRE(c == Fxd{d});
        else
            REQUIRE(c == hi);

    }

}
