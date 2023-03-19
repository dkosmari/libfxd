#include <cfloat>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <limits>

#include <fxd/fixed.hpp>
#include <fxd/except.hpp>

#include "setup.hpp"


using std::overflow_error;
using std::underflow_error;

using fxd::except::from_raw;
using fxd::except::make_fixed;
using fxd::except::make_ufixed;


TEST_CASE("from_raw_signed")
{
    using Fxd = fxd::fixed<8, 4>;
    std::uint32_t u;

    u = 0xff'f;
    CHECK_THROWS_AS(from_raw<Fxd>(u), overflow_error);

    u = 0x80'0;
    CHECK_THROWS_AS(from_raw<Fxd>(u), overflow_error);

    u = 0x7f'f;
    CHECK_NOTHROW(from_raw<Fxd>(u));

    std::int32_t i;
    i = 0xff'f;
    CHECK_THROWS_AS(from_raw<Fxd>(i), overflow_error);

    i = 0x7f'f;
    CHECK_NOTHROW(from_raw<Fxd>(i));

    // this won't throw, as the extra bits are just the sign bit.
    i = 0xfffff'ff'f;
    CHECK_NOTHROW(from_raw<Fxd>(i));

    i = 0xfffff'80'0;
    CHECK_NOTHROW(from_raw<Fxd>(i));

    i = 0xffffe'ff'f;
    CHECK_THROWS_AS(from_raw<Fxd>(i), std::underflow_error);
}


TEST_CASE("from_raw_unsigned")
{
    using Fxd = fxd::ufixed<8, 4>;
    std::uint32_t u;

    u = 0xff'f;
    CHECK_NOTHROW(from_raw<Fxd>(u));

    u = 0x7f'f;
    CHECK_NOTHROW(from_raw<Fxd>(u));

    std::int32_t i;
    i = 0xff'f;
    CHECK_NOTHROW(from_raw<Fxd>(i));

    i = 0xfffff'ff'f;
    CHECK_THROWS_AS(from_raw<Fxd>(i), underflow_error);
}


TEST_CASE("make_fixed<int>")
{
    using Fxd = fxd::fixed<8, 4>;

    CHECK_NOTHROW(make_fixed<Fxd>(127));
    CHECK(make_fixed<Fxd>(127) == 127);
    CHECK_NOTHROW(make_fixed<Fxd>(127u));
    CHECK_NOTHROW(make_fixed<Fxd, std::int8_t>(127));
    CHECK_NOTHROW((make_fixed<Fxd, std::uint8_t>(127u)));
    CHECK_NOTHROW((make_fixed<Fxd, std::uint64_t>(127u)));
    CHECK_NOTHROW((make_fixed<8, 4>(127)));

    CHECK_THROWS_AS(make_fixed<Fxd>(128), overflow_error);
    CHECK_THROWS_AS(make_fixed<Fxd>(128u), overflow_error);
    CHECK_THROWS_AS((make_fixed<Fxd, std::int16_t>(128)), overflow_error);
    CHECK_THROWS_AS((make_fixed<Fxd, std::uint16_t>(128u)), overflow_error);
    CHECK_THROWS_AS((make_fixed<Fxd, std::uint64_t>(128u)), overflow_error);

    CHECK_NOTHROW(make_fixed<Fxd>(-128));
    CHECK(make_fixed<Fxd>(-128) == -128);
    CHECK_NOTHROW(make_fixed<Fxd, std::int8_t>(-128));
    CHECK_THROWS_AS(make_fixed<Fxd>(-129), underflow_error);

    using Fxd2 = fxd::fixed<20, -4>;
    CHECK_NOTHROW(make_fixed<Fxd2>(1024));
    CHECK(make_fixed<Fxd2>(1024) == 1024);
    CHECK_NOTHROW(make_fixed<Fxd2>(-1024));
    CHECK(make_fixed<Fxd2>(-1024) == -1024);
    CHECK_NOTHROW(make_fixed<Fxd2>(-1023));
    CHECK(make_fixed<Fxd2>(-1023) == -1008);
}


TEST_CASE("make_ufixed<int>")
{
    CHECK_NOTHROW((make_ufixed<8, 4>(255 )));
    CHECK_NOTHROW((make_ufixed<8, 4>(255u)));
    CHECK_NOTHROW((make_ufixed<8, 4>(static_cast<std::uint8_t >(255u))));
    CHECK_NOTHROW((make_ufixed<8, 4>(static_cast<std::uint64_t>(255u))));
    CHECK_NOTHROW((make_ufixed<fxd::ufixed<8, 4>>(255 )));

    CHECK_THROWS_AS((make_ufixed<8, 4>(256 )), overflow_error);
    CHECK_THROWS_AS((make_ufixed<8, 4>(256u)), overflow_error);

    CHECK_THROWS_AS((make_ufixed<8, 4>(static_cast<std::int16_t >(256 ))), overflow_error);
    CHECK_THROWS_AS((make_ufixed<8, 4>(static_cast<std::uint16_t>(256u))), overflow_error);
    CHECK_THROWS_AS((make_ufixed<8, 4>(static_cast<std::uint64_t>(256u))), overflow_error);

    CHECK_THROWS_AS((make_ufixed<8, 4>(  -1)), underflow_error);
    CHECK_THROWS_AS((make_ufixed<8, 4>(-128)), underflow_error);
    CHECK_THROWS_AS((make_ufixed<8, 4>(static_cast<std::int8_t>(-128))), underflow_error);

    // test a huge shift
    CHECK_THROWS_AS((make_ufixed<0, 64>(1)), overflow_error);
}


TEST_CASE("make_fixed<float>")
{
    using Fxd = fxd::fixed<8, 4>;
    float f;

    f = std::numeric_limits<float>::infinity();
    CHECK_THROWS_AS(make_fixed<Fxd>(f), overflow_error);
    CHECK_THROWS_AS(make_fixed<Fxd>(-f), underflow_error);

    f = std::numeric_limits<float>::quiet_NaN();
    CHECK_THROWS_AS(make_fixed<Fxd>(f), std::invalid_argument);

    const float high = static_cast<float>(std::numeric_limits<Fxd>::max());
    const float low  = static_cast<float>(std::numeric_limits<Fxd>::lowest());
    CHECK_NOTHROW(make_fixed<Fxd>(high));
    CHECK_NOTHROW(make_fixed<Fxd>(low));

    f = std::nextafter(high, 128.0f);
    CHECK_THROWS_AS(make_fixed<Fxd>(f), overflow_error);
    f = std::nextafter(low, -129.0f);
    CHECK_THROWS_AS(make_fixed<Fxd>(f), underflow_error);
}


TEST_CASE("make_ufixed<float>")
{
    using Fxd = fxd::ufixed<8, 4>;
    float f;

    f = std::numeric_limits<float>::infinity();
    CHECK_THROWS_AS((make_ufixed<8, 4>( f)), overflow_error);
    CHECK_THROWS_AS((make_ufixed<8, 4>(-f)), underflow_error);

    f = std::numeric_limits<float>::quiet_NaN();
    CHECK_THROWS_AS((make_ufixed<8, 4>(f)), std::invalid_argument);

    const float high = static_cast<float>(std::numeric_limits<Fxd>::max());
    CHECK_NOTHROW((make_ufixed<8, 4>(high)));

    f = std::nextafter(high, 256.0f);
    CHECK_THROWS_AS((make_ufixed<8, 4>(f)), overflow_error);
    f = std::nextafter(0.0f, -1.0f);
    CHECK_THROWS_AS((make_ufixed<8, 4>(f)), underflow_error);
}


TEST_CASE("fixed_cast")
{
    using SA = fxd::fixed<8, 24>;
    using SB = fxd::fixed<24, 8>;

    using UA = fxd::ufixed<8, 24>;
    using UB = fxd::ufixed<24, 8>;

    CHECK(fxd::except::fixed_cast<SA>(SB{1}) == SA{1});
    CHECK(fxd::except::fixed_cast<SB>(SA{1}) == SB{1});

    CHECK(fxd::except::fixed_cast<UA>(UB{1}) == UA{1});
    CHECK(fxd::except::fixed_cast<UB>(UA{1}) == UB{1});

    // zero never overflows during cast
    CHECK_NOTHROW((fxd::except::fixed_cast<fxd::fixed<65, -1>>(fxd::fixed<-1, 65>{0})));

    // negative can never cast to unsigned
    CHECK_THROWS_AS(fxd::except::fixed_cast<UA>(SA{-1}), underflow_error);

    // unsigned to signed, can overflow at max
    CHECK_THROWS_AS(fxd::except::fixed_cast<SA>(UA{0x80p0}), overflow_error);

    // no error when losing least significant bits
    SA sa = make_fixed<SA>(0x7f.ff'ff'ffp0);
    CHECK(fxd::except::fixed_cast<SB>(sa) == 0x7f.ffp0);
    sa = make_fixed<SA>(-0x80p0);
    CHECK(fxd::except::fixed_cast<SB>(sa) == -0x80p0);

    // error when losing most significant bits
    SB sb = make_fixed<SB>(0x80p0);
    CHECK_THROWS_AS(fxd::except::fixed_cast<SA>(sb), overflow_error);
    sb = make_fixed<SB>(-0x100p0);
    CHECK_THROWS_AS(fxd::except::fixed_cast<SA>(sb), underflow_error);

    // no error when losing least significant bits
    UA ua = make_fixed<UA>(0xff.ff'ff'ffp0);
    CHECK(fxd::except::fixed_cast<UB>(ua) == 0xff.ffp0);

    // error when losing most significant bits
    UB ub = make_fixed<UB>(0xff.ff'ff'ffp0);
    CHECK_THROWS_AS(fxd::except::fixed_cast<SA>(ub), overflow_error);

    // check that overloads compile
    CHECK(fxd::except::fixed_cast<SB>(sa) == fxd::except::fixed_cast<24, 8>(sa));
    CHECK(fxd::except::ufixed_cast<UB>(ua) == fxd::except::ufixed_cast<24, 8>(ua));

    // force an overflow by doing a huge shift
    CHECK_THROWS_AS((fxd::except::fixed_cast<-63, 127>(fxd::fixed<127, -63>::from_raw(1))),
                    overflow_error);
}


TEST_CASE("to_int")
{
    {
        using Fxd = fxd::fixed<24, 8>;

        Fxd a = 1;
        CHECK(fxd::except::to_int(a) == 1);

        Fxd b = fxd::except::make_fixed<Fxd>(0x7fffff);
        CHECK_THROWS_AS(fxd::except::to_int<std::int16_t>(b), overflow_error);

        Fxd c = -10;
        CHECK(fxd::except::to_int(c) == -10);
        CHECK_THROWS_AS(fxd::except::to_int<std::uint32_t>(c), underflow_error);

        Fxd d = fxd::except::make_fixed<Fxd>(-0x800000);
        CHECK(fxd::except::to_int(d) == -0x800000);
        CHECK_THROWS_AS(fxd::except::to_int<std::int16_t>(d), underflow_error);
    }

    {
        using Fxd = fxd::fixed<20, -4>;

        Fxd a = fxd::except::make_fixed<Fxd>(0x7fff0);
        CHECK(fxd::except::to_int(a) == 0x7fff0);
        CHECK_THROWS_AS(fxd::except::to_int<std::int16_t>(a), overflow_error);

        Fxd b = fxd::except::make_fixed<Fxd>(-0x80000);
        CHECK_THROWS_AS(fxd::except::to_int<std::int16_t>(b), underflow_error);
    }

    {
        using Fxd = fxd::fixed<65, -1>;

        Fxd a = std::numeric_limits<Fxd>::max();
        CHECK_THROWS_AS(fxd::except::to_int<std::int64_t>(a), overflow_error);

        Fxd b = std::numeric_limits<Fxd>::lowest();
        CHECK_THROWS_AS(fxd::except::to_int<std::int64_t>(b), underflow_error);
    }
}


TEST_CASE("assign")
{
    using Fxd = fxd::fixed<8, 24>;

    Fxd a;
    CHECK_NOTHROW(fxd::except::assign(a, 5));
    CHECK_NOTHROW(fxd::except::assign(a, 2.5));
    CHECK_NOTHROW(fxd::except::assign(a, 3.5f));
    Fxd b;
    CHECK_NOTHROW(fxd::except::assign(b, a));

    CHECK_THROWS_AS(fxd::except::assign(a, 128), overflow_error);
    CHECK_THROWS_AS(fxd::except::assign(a, -129), underflow_error);
}


TEST_CASE("inc")
{
    using SFxd = fxd::fixed<8, 24>;

    SFxd a = 126;
    CHECK_NOTHROW(fxd::except::pre_inc(a));
    CHECK(a == 127);
    CHECK_THROWS_AS(fxd::except::pre_inc(a), overflow_error);
    CHECK(a == 127);

    using UFxd = fxd::ufixed<8, 24>;
    UFxd b = 254;
    CHECK_NOTHROW(fxd::except::pre_inc(b));
    CHECK(b == 255);
    CHECK_THROWS_AS(fxd::except::pre_inc(b), overflow_error);
    CHECK(b == 255);

    using UFxd2 = fxd::ufixed<8, 1>;
    UFxd2 c = 254;
    CHECK_NOTHROW(fxd::except::pre_inc(c));
    CHECK(c == 255);
    CHECK_THROWS_AS(fxd::except::pre_inc(c), overflow_error);
    CHECK(c == 255);
}


TEST_CASE("post_inc")
{
    using SFxd = fxd::fixed<8, 24>;

    SFxd a = 126;
    CHECK_NOTHROW(fxd::except::post_inc(a));
    CHECK(a == 127);
    CHECK_THROWS_AS(fxd::except::post_inc(a), overflow_error);
    CHECK(a == 127);

    using UFxd = fxd::ufixed<8, 24>;
    UFxd b = 254;
    CHECK_NOTHROW(fxd::except::post_inc(b));
    CHECK(b == 255);
    CHECK_THROWS_AS(fxd::except::post_inc(b), overflow_error);
    CHECK(b == 255);

    using UFxd2 = fxd::ufixed<8, 1>;
    UFxd2 c = 254;
    CHECK_NOTHROW(fxd::except::post_inc(c));
    CHECK(c == 255);
    CHECK_THROWS_AS(fxd::except::post_inc(c), overflow_error);
    CHECK(c == 255);
}


TEST_CASE("dec")
{
    using SFxd = fxd::fixed<8, 24>;

    SFxd a = -127;
    CHECK_NOTHROW(fxd::except::pre_dec(a));
    CHECK(a == -128);
    CHECK_THROWS_AS(fxd::except::pre_dec(a), underflow_error);
    CHECK(a == -128);

    using UFxd = fxd::ufixed<8, 24>;
    UFxd b = 1;
    CHECK_NOTHROW(fxd::except::pre_dec(b));
    CHECK(b == 0);
    CHECK_THROWS_AS(fxd::except::pre_dec(b), underflow_error);
    CHECK(b == 0);
}


TEST_CASE("post_dec")
{
    using SFxd = fxd::fixed<8, 24>;

    SFxd a = -127;
    CHECK_NOTHROW(fxd::except::post_dec(a));
    CHECK(a == -128);
    CHECK_THROWS_AS(fxd::except::post_dec(a), underflow_error);
    CHECK(a == -128);

    using UFxd = fxd::ufixed<8, 24>;
    UFxd b = 1;
    CHECK_NOTHROW(fxd::except::post_dec(b));
    CHECK(b == 0);
    CHECK_THROWS_AS(fxd::except::post_dec(b), underflow_error);
    CHECK(b == 0);
}


TEST_CASE("negate")
{
    using SFxd = fxd::fixed<8, 24>;

    SFxd a = 127;
    CHECK(fxd::except::negate(a) == -127);
    a = -128;
    CHECK_THROWS_AS(fxd::except::negate(a), overflow_error);

    using UFxd = fxd::ufixed<8, 24>;
    UFxd b = 0;
    CHECK(fxd::except::negate(b) == 0);
    b = 1;
    CHECK_THROWS_AS(fxd::except::negate(b), underflow_error);
}


TEST_CASE("abs")
{
    using Fxd = fxd::fixed<8, 24>;
    Fxd a = 5;
    CHECK(fxd::except::abs(a) == 5);

    Fxd b = -5;
    CHECK(fxd::except::abs(b) == 5);

    Fxd c = std::numeric_limits<Fxd>::lowest();
    CHECK_THROWS_AS(fxd::except::abs(c), overflow_error);
}
