#include <cfenv>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>

#include <fxd/fxd.hpp>

#include "catch2/catch_amalgamated.hpp"

#include "printer.hpp"
#include "rng.hpp"
#include "setup.hpp"

#if defined(__clang__) && __clang_major__ >= 12 && !defined(__FAST_MATH__)
#pragma STDC FENV_ACCESS ON
#endif

TEST_CASE("raw1", "[s8.4]")
{
    using F = fxd::fixed<8, 4>;
    namespace exc = fxd::except;

    std::int16_t s;
    std::uint16_t u;

    s = 0b0000'00000001'0000;
    CHECK(exc::from_raw<F>(s) == F{1});

    s = 0b0000'01111111'0000;
    CHECK(exc::from_raw<F>(s) == F{127});

    s = 0b0000'10000000'0000;
    CHECK_THROWS_AS(exc::from_raw<F>(s), std::overflow_error);

    s = 0b1111'10000000'0000;
    CHECK(exc::from_raw<F>(s) == F{-128});

    --s;
    CHECK_THROWS_AS(exc::from_raw<F>(s), std::underflow_error);

    s = 0b1111'11111111'0000;
    CHECK(exc::from_raw<F>(s) == F{-1});


    u = 0b0000'01111111'0000;
    CHECK(exc::from_raw<F>(u) == F{127});

    u = 0b0000'10000000'0000;
    CHECK_THROWS_AS(exc::from_raw<F>(u), std::overflow_error);

    u = 0b1111'11111111'1111;
    CHECK_THROWS_AS(exc::from_raw<F>(u), std::overflow_error);
}


TEST_CASE("raw2", "[u8.4]")
{
    using F = fxd::ufixed<8, 4>;
    namespace exc = fxd::except;

    std::int16_t s;
    std::uint16_t u;

    s = 0b0000'00000001'0000;
    CHECK(exc::from_raw<F>(s) == F{1});

    s = 0b0000'01111111'0000;
    CHECK(exc::from_raw<F>(s) == F{127});

    s = 0b0000'10000000'0000;
    CHECK(exc::from_raw<F>(s) == F{128});

    s = 0b1111'10000000'0000;
    CHECK_THROWS_AS(exc::from_raw<F>(s), std::underflow_error);

    s = 0b1111'10000000'0000;
    CHECK_THROWS_AS(exc::from_raw<F>(s), std::underflow_error);


    u = 0b0000'01111111'0000;
    CHECK(exc::from_raw<F>(u) == F{127});

    u = 0b0000'10000000'0000;
    CHECK(exc::from_raw<F>(u) == F{128});

    u = 0b0000'11111111'0000;
    CHECK(exc::from_raw<F>(u) == F{255});

    u = 0b1111'11111111'1111;
    CHECK_THROWS_AS(exc::from_raw<F>(u), std::overflow_error);
}


TEST_CASE("raw3", "[s4.4]")
{
    using F = fxd::fixed<4, 4>;
    using Lim = std::numeric_limits<F>;
    namespace exc = fxd::except;

    for (int i = -512; i <= 512; ++i) {

        if (i < Lim::lowest().raw_value)
            CHECK_THROWS_AS(exc::from_raw<F>(i), std::underflow_error);
        else if (i <= Lim::max().raw_value)
            CHECK_NOTHROW(exc::from_raw<F>(i));
        else
            CHECK_THROWS_AS(exc::from_raw<F>(i), std::overflow_error);

    }

    for (unsigned i = 0; i <= 1024; ++i) {

        if (i <= unsigned(Lim::max().raw_value))
            CHECK_NOTHROW(exc::from_raw<F>(i));
        else
            CHECK_THROWS_AS(exc::from_raw<F>(i), std::overflow_error);

    }
}


TEST_CASE("raw4", "[u4.4]")
{
    using F = fxd::ufixed<4, 4>;
    using Lim = std::numeric_limits<F>;
    namespace exc = fxd::except;

    for (int i = -512; i <= 512; ++i) {

        if (i < Lim::lowest().raw_value)
            CHECK_THROWS_AS(exc::from_raw<F>(i), std::underflow_error);
        else if (i <= Lim::max().raw_value)
            CHECK_NOTHROW(exc::from_raw<F>(i));
        else
            CHECK_THROWS_AS(exc::from_raw<F>(i), std::overflow_error);

    }

    for (unsigned i = 0; i <= 1024; ++i) {

        if (i <= Lim::max().raw_value)
            CHECK_NOTHROW(exc::from_raw<F>(i));
        else
            CHECK_THROWS_AS(exc::from_raw<F>(i), std::overflow_error);

    }
}


TEST_CASE("make1", "[s8.4]")
{
    using F = fxd::fixed<8, 4>;
    using Lim = std::numeric_limits<F>;
    namespace exc = fxd::except;

    constexpr F min = Lim::lowest();
    constexpr F max = Lim::max();

    for (int i = -512; i <= 512; ++i) {
        if (i < (min.raw_value >> 4))
            CHECK_THROWS_AS(exc::make_fixed<F>(i), std::underflow_error);
        else if (i <= (max.raw_value >> 4))
            CHECK_NOTHROW(exc::make_fixed<F>(i));
        else
            CHECK_THROWS_AS(exc::make_fixed<F>(i), std::overflow_error);
    }

    for (unsigned i = 0; i <= 1024; ++i) {
        if (i <= unsigned(max.raw_value >> 4))
            CHECK_NOTHROW(exc::make_fixed<F>(i));
        else
            CHECK_THROWS_AS(exc::make_fixed<F>(i), std::overflow_error);
    }
}


TEST_CASE("make2", "[u8.4]")
{
    using F = fxd::ufixed<8, 4>;
    using Lim = std::numeric_limits<F>;
    namespace exc = fxd::except;

    constexpr F min = Lim::lowest();
    constexpr F max = Lim::max();

    for (int i = -512; i <= 512; ++i) {
        if (i < int(min.raw_value >> 4))
            CHECK_THROWS_AS(exc::make_fixed<F>(i), std::underflow_error);
        else if (i <= int(max.raw_value >> 4))
            CHECK_NOTHROW(exc::make_fixed<F>(i));
        else
            CHECK_THROWS_AS(exc::make_fixed<F>(i), std::overflow_error);
    }

    for (unsigned i = 0; i <= 1024; ++i) {
        if (i <= max.raw_value >> 4)
            CHECK_NOTHROW(exc::make_fixed<F>(i));
        else
            CHECK_THROWS_AS(exc::make_fixed<F>(i), std::overflow_error);
    }
}



TEST_CASE("make3", "[s1.31]")
{
    using Fxd = fxd::fixed<1, 31>; // range is [-1, 1)
    using Flt = typename std::numeric_limits<Fxd>::float_type;
    namespace exc = fxd::except;
    using over = std::overflow_error;
    using under = std::underflow_error;

    CHECK_NOTHROW(exc::make_fixed<Fxd>(0));
    CHECK_NOTHROW(exc::make_fixed<Fxd>(0.0f));
    CHECK_NOTHROW(exc::make_fixed<Fxd>(0.0));

    CHECK_THROWS_AS(exc::make_fixed<Fxd>(1), over);
    CHECK_THROWS_AS(exc::make_fixed<Fxd>(1.0f), over);
    CHECK_THROWS_AS(exc::make_fixed<Fxd>(1.0), over);
    CHECK_THROWS_AS(exc::make_fixed<Fxd>(1.0L), over);

    CHECK_NOTHROW(exc::make_fixed<Fxd>(-1));
    CHECK_NOTHROW(exc::make_fixed<Fxd>(-1.0f));
    CHECK_NOTHROW(exc::make_fixed<Fxd>(-1.0));
    CHECK_NOTHROW(exc::make_fixed<Fxd>(-1.0L));

    {
        const Flt a = std::nextafter(1.0f, 0.0f);
        CHECK_NOTHROW(exc::make_fixed<Fxd>(a));
        const long double b = a;
        CHECK_NOTHROW(exc::make_fixed<Fxd>(b));

        // ensure the extra mantissa bits are not being discarded
        long double c = to_float(std::numeric_limits<Fxd>::max());
        CHECK_NOTHROW(exc::make_fixed<Fxd>(c));
        c = std::nextafter(c, 2.0L);
        CHECK_THROWS_AS(exc::make_fixed<Fxd>(c), over);

        c = to_float(std::numeric_limits<Fxd>::lowest());
        CHECK_NOTHROW(exc::make_fixed<Fxd>(c));
        c = std::nextafter(c, -2.0L);
        CHECK_THROWS_AS(exc::make_fixed<Fxd>(c), under);
    }

    {
        const long double a = std::nextafter(-1.0L, -2.0L);
        CHECK_THROWS_AS(exc::make_fixed<Fxd>(a), under);
    }

}



TEST_CASE("make4", "[u1.31]")
{
    using Fxd = fxd::ufixed<1, 31>; // range is [0, 2)
    using Flt = typename std::numeric_limits<Fxd>::float_type;
    namespace exc = fxd::except;
    using over = std::overflow_error;
    using under = std::underflow_error;

    CHECK_NOTHROW(exc::make_fixed<Fxd>(0));
    CHECK_NOTHROW(exc::make_fixed<Fxd>(0.0f));
    CHECK_NOTHROW(exc::make_fixed<Fxd>(0.0));

    CHECK_THROWS_AS(exc::make_fixed<Fxd>(2), over);
    CHECK_THROWS_AS(exc::make_fixed<Fxd>(2.0f), over);
    CHECK_THROWS_AS(exc::make_fixed<Fxd>(2.0), over);
    CHECK_THROWS_AS(exc::make_fixed<Fxd>(2.0L), over);

    CHECK_THROWS_AS(exc::make_fixed<Fxd>(-1), under);
    CHECK_THROWS_AS(exc::make_fixed<Fxd>(-1.0f), under);
    CHECK_THROWS_AS(exc::make_fixed<Fxd>(-1.0), under);
    CHECK_THROWS_AS(exc::make_fixed<Fxd>(-1.0L), under);

    {
        const Flt a = std::nextafter(2.0f, 0.0f);
        CHECK_NOTHROW(exc::make_fixed<Fxd>(a));
        const long double b = a;
        CHECK_NOTHROW(exc::make_fixed<Fxd>(b));

        // ensure the extra mantissa bits are not being discarded
        long double c = to_float(std::numeric_limits<Fxd>::max());
        CHECK_NOTHROW(exc::make_fixed<Fxd>(c));
        c = std::nextafter(c, 2.0L);
        CHECK_THROWS_AS(exc::make_fixed<Fxd>(c), over);

        c = to_float(std::numeric_limits<Fxd>::lowest());
        CHECK_NOTHROW(exc::make_fixed<Fxd>(c));
        c = std::nextafter(c, -2.0L);
        CHECK_THROWS_AS(exc::make_fixed<Fxd>(c), under);
    }

}


TEST_CASE("make5", "[u25.0]")
{
    using Fxd = fxd::ufixed<25, 0>;
    namespace exc = fxd::except;

    const float a = 0x01fffffep0f;
    {
        CAPTURE(a);
        CHECK_NOTHROW(exc::make_fixed<Fxd>(a));
        CHECK(exc::make_fixed<Fxd>(a) < std::numeric_limits<Fxd>::max());
    }

    const float b = std::nextafter(a, 2 * a);
    {
        CAPTURE(b);
        CAPTURE(std::numeric_limits<Fxd>::max());
        CHECK_THROWS_AS(exc::make_fixed<Fxd>(b), std::overflow_error);
    }

    const float c = static_cast<float>(std::numeric_limits<Fxd>::max());
    CHECK(c == a);
}



TEST_CASE("make6", "[s26.0]")
{
    using Fxd = fxd::fixed<26, 0>;
    namespace exc = fxd::except;

    const float a = 0x01fffffep0f;
    {
        CAPTURE(a);
        CAPTURE(std::numeric_limits<Fxd>::max());
        CHECK_NOTHROW(exc::make_fixed<Fxd>(a));
    }

    const float b = std::nextafter(a, 2 * a);
    {
        CAPTURE(b);
        CAPTURE(std::numeric_limits<Fxd>::max());
        CHECK_THROWS_AS(exc::make_fixed<Fxd>(b), std::overflow_error);
    }

    const float c = -0x02000000p0f;
    {
        CAPTURE(c);
        CAPTURE(std::numeric_limits<Fxd>::lowest());
        CHECK_NOTHROW(exc::make_fixed<Fxd>(c));
    }

    const float d = std::nextafter(c, 2 * c);
    {
        CAPTURE(d);
        CAPTURE(std::numeric_limits<Fxd>::lowest());
        CHECK_THROWS_AS(exc::make_fixed<Fxd>(d), std::underflow_error);
    }
}
