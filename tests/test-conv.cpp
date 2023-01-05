#include <cstdint>
#include <limits>
#include <iostream>
#include <cmath>

#include <fxd/fxd.hpp>

#include "catch2/catch_amalgamated.hpp"

#include "printer.hpp"
#include "rng.hpp"
#include "setup.hpp"


TEST_CASE("raw1", "[s8.4]")
{
    using F = fxd::fixed<8, 4>;
    using exc = fxd::safe::except;

    std::int16_t s;
    std::uint16_t u;

    s = 0b0000'00000001'0000;
    REQUIRE(exc::from_raw<F>(s) == F{1});

    s = 0b0000'01111111'0000;
    REQUIRE(exc::from_raw<F>(s) == F{127});

    s = 0b0000'10000000'0000;
    REQUIRE_THROWS_AS(exc::from_raw<F>(s), std::overflow_error);

    s = 0b1111'10000000'0000;
    REQUIRE(exc::from_raw<F>(s) == F{-128});

    --s;
    REQUIRE_THROWS_AS(exc::from_raw<F>(s), std::underflow_error);

    s = 0b1111'11111111'0000;
    REQUIRE(exc::from_raw<F>(s) == F{-1});


    u = 0b0000'01111111'0000;
    REQUIRE(exc::from_raw<F>(u) == F{127});

    u = 0b0000'10000000'0000;
    REQUIRE_THROWS_AS(exc::from_raw<F>(u), std::overflow_error);

    u = 0b1111'11111111'1111;
    REQUIRE_THROWS_AS(exc::from_raw<F>(u), std::overflow_error);
}


TEST_CASE("raw2", "[u8.4]")
{
    using F = fxd::ufixed<8, 4>;
    using exc = fxd::safe::except;

    std::int16_t s;
    std::uint16_t u;

    s = 0b0000'00000001'0000;
    REQUIRE(exc::from_raw<F>(s) == F{1});

    s = 0b0000'01111111'0000;
    REQUIRE(exc::from_raw<F>(s) == F{127});

    s = 0b0000'10000000'0000;
    REQUIRE(exc::from_raw<F>(s) == F{128});

    s = 0b1111'10000000'0000;
    REQUIRE_THROWS_AS(exc::from_raw<F>(s), std::underflow_error);

    s = 0b1111'10000000'0000;
    REQUIRE_THROWS_AS(exc::from_raw<F>(s), std::underflow_error);


    u = 0b0000'01111111'0000;
    REQUIRE(exc::from_raw<F>(u) == F{127});

    u = 0b0000'10000000'0000;
    REQUIRE(exc::from_raw<F>(u) == F{128});

    u = 0b0000'11111111'0000;
    REQUIRE(exc::from_raw<F>(u) == F{255});

    u = 0b1111'11111111'1111;
    REQUIRE_THROWS_AS(exc::from_raw<F>(u), std::overflow_error);
}


TEST_CASE("raw3", "[s8.4]")
{
    using F = fxd::fixed<8, 4>;
    using Lim = std::numeric_limits<F>;
    using exc = fxd::safe::except;

    for (int i = -512; i <= 512; ++i) {

        if (i < Lim::lowest().raw_value)
            REQUIRE_THROWS_AS(exc::from_raw<F>(i), std::underflow_error);
        else if (i <= Lim::max().raw_value)
            REQUIRE_NOTHROW(exc::from_raw<F>(i));
        else
            REQUIRE_THROWS_AS(exc::from_raw<F>(i), std::overflow_error);

    }

    for (unsigned i = 0; i <= 1024; ++i) {

        if (i <= unsigned(Lim::max().raw_value))
            REQUIRE_NOTHROW(exc::from_raw<F>(i));
        else
            REQUIRE_THROWS_AS(exc::from_raw<F>(i), std::overflow_error);

    }
}


TEST_CASE("raw4", "[u8.4]")
{
    using F = fxd::ufixed<8, 4>;
    using Lim = std::numeric_limits<F>;
    using exc = fxd::safe::except;

    for (int i = -512; i <= 512; ++i) {

        if (i < Lim::lowest().raw_value)
            REQUIRE_THROWS_AS(exc::from_raw<F>(i), std::underflow_error);
        else if (i <= Lim::max().raw_value)
            REQUIRE_NOTHROW(exc::from_raw<F>(i));
        else
            REQUIRE_THROWS_AS(exc::from_raw<F>(i), std::overflow_error);

    }

    for (unsigned i = 0; i <= 1024; ++i) {

        if (i <= Lim::max().raw_value)
            REQUIRE_NOTHROW(exc::from_raw<F>(i));
        else
            REQUIRE_THROWS_AS(exc::from_raw<F>(i), std::overflow_error);

    }
}


TEST_CASE("make1", "[s8.4]")
{
    using F = fxd::fixed<8, 4>;
    using Lim = std::numeric_limits<F>;
    using exc = fxd::safe::except;

    constexpr F min = Lim::lowest();
    constexpr F max = Lim::max();

    for (int i = -512; i <= 512; ++i) {
        if (i < (min.raw_value >> 4))
            REQUIRE_THROWS_AS(exc::make_fixed<F>(i), std::underflow_error);
        else if (i <= (max.raw_value >> 4))
            REQUIRE_NOTHROW(exc::make_fixed<F>(i));
        else
            REQUIRE_THROWS_AS(exc::make_fixed<F>(i), std::overflow_error);
    }

    for (unsigned i = 0; i <= 1024; ++i) {
        if (i <= unsigned(max.raw_value >> 4))
            REQUIRE_NOTHROW(exc::make_fixed<F>(i));
        else
            REQUIRE_THROWS_AS(exc::make_fixed<F>(i), std::overflow_error);
    }
}


TEST_CASE("make2", "[u8.4]")
{
    using F = fxd::ufixed<8, 4>;
    using Lim = std::numeric_limits<F>;
    using exc = fxd::safe::except;

    constexpr F min = Lim::lowest();
    constexpr F max = Lim::max();

    for (int i = -512; i <= 512; ++i) {
        if (i < int(min.raw_value >> 4))
            REQUIRE_THROWS_AS(exc::make_fixed<F>(i), std::underflow_error);
        else if (i <= int(max.raw_value >> 4))
            REQUIRE_NOTHROW(exc::make_fixed<F>(i));
        else
            REQUIRE_THROWS_AS(exc::make_fixed<F>(i), std::overflow_error);
    }

    for (unsigned i = 0; i <= 1024; ++i) {
        if (i <= max.raw_value >> 4)
            REQUIRE_NOTHROW(exc::make_fixed<F>(i));
        else
            REQUIRE_THROWS_AS(exc::make_fixed<F>(i), std::overflow_error);
    }
}



TEST_CASE("make3", "[s1.31]")
{
    using Fxd = fxd::fixed<1, 31>; // range is [-1, 1)
    using Flt = Fxd::float_type;
    using exc = fxd::safe::except;
    using std::nextafter;
    using over = std::overflow_error;
    using under = std::underflow_error;

    REQUIRE_NOTHROW(exc::make_fixed<Fxd>(0));
    REQUIRE_NOTHROW(exc::make_fixed<Fxd>(0.0f));
    REQUIRE_NOTHROW(exc::make_fixed<Fxd>(0.0));

    REQUIRE_THROWS_AS(exc::make_fixed<Fxd>(1), over);
    REQUIRE_THROWS_AS(exc::make_fixed<Fxd>(1.0f), over);
    REQUIRE_THROWS_AS(exc::make_fixed<Fxd>(1.0), over);
    REQUIRE_THROWS_AS(exc::make_fixed<Fxd>(1.0L), over);

    REQUIRE_NOTHROW(exc::make_fixed<Fxd>(-1));
    REQUIRE_NOTHROW(exc::make_fixed<Fxd>(-1.0f));
    REQUIRE_NOTHROW(exc::make_fixed<Fxd>(-1.0));
    REQUIRE_NOTHROW(exc::make_fixed<Fxd>(-1.0L));

    {
        const Flt a = nextafter(1.0f, 0.0f);
        REQUIRE_NOTHROW(exc::make_fixed<Fxd>(a));
        const long double b = a;
        REQUIRE_NOTHROW(exc::make_fixed<Fxd>(b));

        // ensure the extra mantissa bits are not being discarded
        long double c = to_float(std::numeric_limits<Fxd>::max());
        REQUIRE_NOTHROW(exc::make_fixed<Fxd>(c));
        c = nextafter(c, 2.0L);
        REQUIRE_THROWS_AS(exc::make_fixed<Fxd>(c), over);

        c = to_float(std::numeric_limits<Fxd>::lowest());
        REQUIRE_NOTHROW(exc::make_fixed<Fxd>(c));
        c = nextafter(c, -2.0L);
        REQUIRE_THROWS_AS(exc::make_fixed<Fxd>(c), under);
    }

    {
        const long double a = nextafter(-1.0L, -2.0L);
        REQUIRE_THROWS_AS(exc::make_fixed<Fxd>(a), under);
    }

}



TEST_CASE("make4", "[u1.31]")
{
    using Fxd = fxd::ufixed<1, 31>; // range is [0, 2)
    using Flt = Fxd::float_type;
    using exc = fxd::safe::except;
    using std::nextafter;
    using over = std::overflow_error;
    using under = std::underflow_error;

    REQUIRE_NOTHROW(exc::make_fixed<Fxd>(0));
    REQUIRE_NOTHROW(exc::make_fixed<Fxd>(0.0f));
    REQUIRE_NOTHROW(exc::make_fixed<Fxd>(0.0));

    REQUIRE_THROWS_AS(exc::make_fixed<Fxd>(2), over);
    REQUIRE_THROWS_AS(exc::make_fixed<Fxd>(2.0f), over);
    REQUIRE_THROWS_AS(exc::make_fixed<Fxd>(2.0), over);
    REQUIRE_THROWS_AS(exc::make_fixed<Fxd>(2.0L), over);

    REQUIRE_THROWS_AS(exc::make_fixed<Fxd>(-1), under);
    REQUIRE_THROWS_AS(exc::make_fixed<Fxd>(-1.0f), under);
    REQUIRE_THROWS_AS(exc::make_fixed<Fxd>(-1.0), under);
    REQUIRE_THROWS_AS(exc::make_fixed<Fxd>(-1.0L), under);

    {
        const Flt a = nextafter(2.0f, 0.0f);
        REQUIRE_NOTHROW(exc::make_fixed<Fxd>(a));
        const long double b = a;
        REQUIRE_NOTHROW(exc::make_fixed<Fxd>(b));

        // ensure the extra mantissa bits are not being discarded
        long double c = to_float(std::numeric_limits<Fxd>::max());
        REQUIRE_NOTHROW(exc::make_fixed<Fxd>(c));
        c = nextafter(c, 2.0L);
        REQUIRE_THROWS_AS(exc::make_fixed<Fxd>(c), over);

        c = to_float(std::numeric_limits<Fxd>::lowest());
        REQUIRE_NOTHROW(exc::make_fixed<Fxd>(c));
        c = nextafter(c, -2.0L);
        REQUIRE_THROWS_AS(exc::make_fixed<Fxd>(c), under);
    }

}
