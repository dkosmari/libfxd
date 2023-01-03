#include <cstdlib>
#include <cxxabi.h>
#include <string>
#include <tuple>

#include "catch2/catch_amalgamated.hpp"

#include <fxd/fxd.hpp>

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


#if 1
TEST_CASE("special1")
{
    using Fxd = fxd::fixed<-1, 65>;
    using Flt = long double;

    Fxd a = Fxd::from_raw(0x0db0a5a5d9d2167e);
    Fxd b = Fxd::from_raw(0x5f80ef4415b492ad);
    Fxd c = a / b;
    SHOW(a);
    SHOW(b);
    SHOW(c);

    Flt fa = static_cast<Flt>(a);
    Flt fb = static_cast<Flt>(b);
    Flt fc = static_cast<Flt>(c);
    Flt fd = fa / fb;
    // Flt fd = static_cast<__float128>(fa) / static_cast<__float128>(fb);

    std::cout << std::setprecision(30)
              << std::fixed;

    auto show = [](const char* name, Flt x)
    {
        std::cout << name
                  << " = "
                  << std::setw(30 + 5)
                  << x
                  << '\n';
    };

    show("fa", fa);
    show("fb", fb);
    show("fc", fc);
    show("fd", fd);

    Fxd d = fd;
    SHOW(d);

    Fxd e = b * c;
    SHOW(a);
    SHOW(e);
    Fxd f = b * d;
    SHOW(f);

    REQUIRE(c == d);
}
#endif


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
    tuple<fxd::fixed<1, 63>,   long double>,
    tuple<fxd::fixed<0, 64>,   long double>,

    tuple<fxd::ufixed<32, 32>, long double>,
    tuple<fxd::ufixed<63, 1>,  long double>,
    tuple<fxd::ufixed<64, 0>,  long double>,
    tuple<fxd::ufixed<1, 63>,  long double>,
    tuple<fxd::ufixed<0, 64>,  long double>
    >;


TEMPLATE_LIST_TEST_CASE("random-basic",
                        "[random][raw]",
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

        Flt fa = static_cast<Flt>(a);
        Flt fb = static_cast<Flt>(b);
        Flt fc = fa / fb;
        if (b && flo <= fc && fc <= fhi) {
            Fxd c = a / b;

            Fxd cc{fc};

            REQUIRE(c == cc);
        }
    }
}


TEMPLATE_LIST_TEST_CASE("random-sat",
                        "[saturation][random]",
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
        if (b) {

            Fxd c = fxd::safe::saturate::divides(a, b);

            Flt fa = static_cast<Flt>(a);
            Flt fb = static_cast<Flt>(b);
            Flt d = fa / fb;
            if (!std::isfinite(d))
                std::cerr << "not-finite result: " << d << std::endl;

            if (d < flo)
                REQUIRE(c == lo);
            else if (d < fhi)
                REQUIRE(c == Fxd{d});
            else
                REQUIRE(c == hi);
        }
    }
 }
