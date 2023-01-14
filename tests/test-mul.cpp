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


TEST_CASE("special1")
{
    using F = fxd::fixed<33, 20>;

    std::int64_t ra = 0xfff99a67f370c65aLL;
    std::int64_t rb = 0x0002bd0e4b41fb2dLL;

    F a = F::from_raw(ra);
    CAPTURE(a);

    F b = F::from_raw(rb);
    CAPTURE(b);

    F ab = fxd::safe::saturate::mul(a, b);
    F lo = std::numeric_limits<F>::lowest();

    CHECK(ab == lo);
}


TEST_CASE("special2")
{
    using std::cout;
    using std::endl;

    using I = std::int64_t;

    I a = 0xfff99a67f370c65a;
    I b = 0x0002bd0e4b41fb2d;
    auto c = fxd::utils::mul::mul(a, b);
    CHECK(get<0>(c) == 0x814a34a018271bd2ULL);
    CHECK(get<1>(c) == static_cast<I>(0xffffffee7b7335e1LL));
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
    CHECK(c == F{d});
}



TEMPLATE_LIST_TEST_CASE("random-basic",
                        "",
                        test_types)
{
    using Fxd = TestType;
    using Flt = Fxd::float_type;

    constexpr Fxd lo = std::numeric_limits<Fxd>::lowest();
    constexpr Fxd hi = std::numeric_limits<Fxd>::max();

    const Flt flo = static_cast<Flt>(lo);
    const Flt fhi = static_cast<Flt>(hi);

    RNG<Fxd> rng;

    for (int i = 0; i < 10000; ++i) {

        Fxd a = rng.get();
        Fxd b = rng.get();

        Flt d = static_cast<Flt>(a) * static_cast<Flt>(b);
        if (flo <= d && d <= fhi) {
            Fxd c = a * b;

            REQUIRE(c == Fxd{d});
        }

    }
}


TEMPLATE_LIST_TEST_CASE("random-safe",
                        "[safe]",
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
        Fxd c = fxd::safe::saturate::mul(a, b);
        Flt d = static_cast<Flt>(a) * static_cast<Flt>(b);

        CAPTURE(a);
        CAPTURE(b);
        CAPTURE(c);
        CAPTURE(d);

        if (d < flo) {
            REQUIRE(c == lo);
            REQUIRE_THROWS_AS(fxd::safe::except::mul(a, b), std::underflow_error);
        } else if (d <= fhi) {
            REQUIRE(c == Fxd{d});
        } else {
            REQUIRE(c == hi);
            REQUIRE_THROWS_AS(fxd::safe::except::mul(a, b), std::overflow_error);
        }

    }

}
