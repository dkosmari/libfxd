#include <cstdlib>
#include <string>

#include <fxd/fxd.hpp>

#include "catch2/catch_amalgamated.hpp"

#include "printer.hpp"
#include "rng.hpp"
#include "setup.hpp"
#include "test-types.hpp"



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


TEMPLATE_LIST_TEST_CASE("random-basic",
                        "[unsafe][zero]",
                        test_types)
{
    using Fxd = TestType;

    constexpr Fxd lo = std::numeric_limits<Fxd>::lowest();
    constexpr Fxd hi = std::numeric_limits<Fxd>::max();

    const auto flo = to_float(lo);
    const auto fhi = to_float(hi);

    RNG<Fxd> rng;

    for (int i = 0; i < 10000; ++i) {

        Fxd a = rng.get();
        Fxd b = rng.get();
        CAPTURE(a);
        CAPTURE(b);

        auto ab = to_float(a) * to_float(b);
        if (flo <= ab && ab <= fhi) {
            Fxd c = a * b;
            REQUIRE(c == Fxd{ab});

            Fxd sat_c = fxd::saturate::mul(a, b);
            REQUIRE(sat_c == Fxd{ab});

            REQUIRE_NOTHROW(fxd::except::mul(a, b));
        }

    }
}


TEMPLATE_LIST_TEST_CASE("random-safe",
                        "[safe]",
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
        Fxd c = fxd::saturate::mul(a, b);
        auto ab = to_float(a) * to_float(b);

        CAPTURE(a);
        CAPTURE(b);
        CAPTURE(c);
        CAPTURE(ab);

        if (ab < flo) {

            REQUIRE(c == lo);
            REQUIRE_THROWS_AS(fxd::except::mul(a, b), std::underflow_error);

        } else if (ab <= fhi) {

            REQUIRE(c == Fxd{ab});

        } else {

            REQUIRE(c == hi);
            REQUIRE_THROWS_AS(fxd::except::mul(a, b), std::overflow_error);

        }

    }

}


TEMPLATE_LIST_TEST_CASE("random-basic-up",
                        "[unsafe][up]",
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

    fxd::utils::round_up guard;

    for (int i = 0; i < 10000; ++i) {

        Fxd a = rng.get();
        Fxd b = rng.get();
        CAPTURE(a);
        CAPTURE(b);

        auto ab = to_float(a) * to_float(b);
        CAPTURE(ab);

        if (flo <= ab && ab <= fhi) {
            Fxd c = fxd::round::up::mul(a, b);
            REQUIRE(c == Fxd{ab});
        }

    }
}


TEMPLATE_LIST_TEST_CASE("random-basic-down",
                        "[unsafe][down]",
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

    fxd::utils::round_down guard;

    for (int i = 0; i < 10000; ++i) {

        Fxd a = rng.get();
        Fxd b = rng.get();
        CAPTURE(a);
        CAPTURE(b);

        auto ab = to_float(a) * to_float(b);
        CAPTURE(ab);

        if (flo <= ab && ab <= fhi) {
            Fxd c = fxd::round::down::mul(a, b);
            REQUIRE(c == Fxd{ab});
        }

    }
}



TEST_CASE("special-1")
{
    using F = fxd::fixed<33, 20>;

    std::int64_t ra = 0xfff99a67f370c65aLL;
    std::int64_t rb = 0x0002bd0e4b41fb2dLL;

    F a = F::from_raw(ra);
    CAPTURE(a);

    F b = F::from_raw(rb);
    CAPTURE(b);

    F ab = fxd::saturate::mul(a, b);
    F lo = std::numeric_limits<F>::lowest();

    CHECK(ab == lo);
}


TEST_CASE("special-2")
{
    using std::cout;
    using std::endl;

    using I = std::int64_t;

    I a = 0xfff99a67f370c65a;
    I b = 0x0002bd0e4b41fb2d;
    auto c = fxd::utils::mul::mul<64>(a, b);
    CHECK(get<0>(c) == 0x814a34a018271bd2ULL);
    CHECK(get<1>(c) == static_cast<I>(0xffffffee7b7335e1LL));
}


TEST_CASE("special-3")
{
    using F = fxd::fixed<-1, 65>;

    F a = F::from_raw(0x679d16dc561eef4d); // 0.202370371243032804
    F b = F::from_raw(0xabafbb5b0dac6969); // -0.164674897322732484
    F c = a * b;
    // SHOW(a);
    // SHOW(b);
    // SHOW(c);
    long double d = to_float(a) * to_float(b);
    CHECK(c == F{d});
}


TEST_CASE("special-4")
{
    using F = fxd::fixed<-1, 65>;

    F a = F::from_raw(893234026250213348LL);
    F b = F::from_raw(-5616900536081592863LL);
    F c = a * b;
    F d = F::from_raw(-135991659580774104LL);
    CHECK(c == d);
    F e = fxd::saturate::mul(a, b);
    CHECK(e == d);
}


TEST_CASE("special-5", "[up]")
{
    using Fxd = fxd::fixed<13, 12>;

    fxd::utils::round_up guard;

    Fxd a = Fxd::from_raw(2832);
    Fxd b = Fxd::from_raw(14248059);
    Fxd c = fxd::round::up::mul(a, b);
    auto fa = to_float(a);
    auto fb = to_float(b);
    auto fc = fa * fb;
    CAPTURE(a);
    CAPTURE(fa);
    CAPTURE(b);
    CAPTURE(fb);
    CAPTURE(c);
    CAPTURE(fc);
    CHECK(c == Fxd{fc});
}


TEST_CASE("special-6", "[up]")
{
    using Fxd = fxd::fixed<13, 12>;

    fxd::utils::round_up guard;

    Fxd a = Fxd::from_raw(8775);
    Fxd b = Fxd::from_raw(88974);
    Fxd c = fxd::round::up::mul(a, b);
    auto fa = to_float(a);
    auto fb = to_float(b);
    auto fc = fa * fb;
    CAPTURE(a);
    CAPTURE(fa);
    CAPTURE(b);
    CAPTURE(fb);
    CAPTURE(c);
    CAPTURE(fc);
    CHECK(c == Fxd{fc});
}


TEST_CASE("special-7", "[down]")
{
    using Fxd = fxd::fixed<13, 12>;

    fxd::utils::round_down guard;

    Fxd a = Fxd::from_raw(-1014032);
    Fxd b = Fxd::from_raw(-4055);
    Fxd c = fxd::round::down::mul(a, b);
    auto fa = to_float(a);
    auto fb = to_float(b);
    auto fc = fa * fb;
    CAPTURE(a);
    CAPTURE(fa);
    CAPTURE(b);
    CAPTURE(fb);
    CAPTURE(c);
    CAPTURE(fc);
    CHECK(c == Fxd{fc});
}

/*
test-mul.cpp:387: FAILED:
  REQUIRE( c == hi )
with expansion:
  -4096.0_fix<13,12>  [ -16777216 ]
  ==
  4095.99975_fix<13,12>  [ 16777215 (0xffffff) ]
with messages:
  lo := -4096.0_fix<13,12>  [ -16777216 ]
  hi := 4095.99975_fix<13,12>  [ 16777215 (0xffffff) ]
  flo := -4096.0f
  fhi := 4095.999755859f
  a := 3217.23339_fix<13,12>  [ 13177788 (0xc913bc) ]
  b := 2678.71362_fix<13,12>  [ 10972011 (0xa76b6b) ]
  c := -4096.0_fix<13,12>  [ -16777216 ]
  ab := 8618046.0f
 */


TEST_CASE("special-8")
{
    using Fxd = fxd::fixed<13, 12>;

    Fxd a = Fxd::from_raw(13177788);
    Fxd b = Fxd::from_raw(10972011);
    Fxd c = fxd::saturate::mul(a, b);
    CAPTURE(a);
    CAPTURE(b);
    CHECK(c == std::numeric_limits<Fxd>::max());
    CHECK_THROWS_AS(fxd::except::mul(a, b), std::overflow_error);
}
