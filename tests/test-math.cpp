#include <cerrno>
#include <cmath>

#include <fxd/limits.hpp>
#include <fxd/math.hpp>

#include "setup.hpp"


TEST_CASE("abs")
{
    using Fxd = fxd::fixed<8, 10>;

    Fxd a = 3;
    Fxd b = -3;
    CHECK(abs(a) == 3);
    CHECK(abs(b) == 3);
    CHECK(abs(a) == abs(b));
}


TEST_CASE("fdim")
{
    using Fxd = fxd::fixed<8, 10>;

    Fxd a = 13;
    Fxd b = 11;

    CHECK(fdim(a, b) == 2);
    CHECK(fdim(b, a) == 0);
}


TEST_CASE("nextafter")
{
    using Fxd = fxd::fixed<8, 10>;
    constexpr Fxd eps = std::numeric_limits<Fxd>::epsilon();

    Fxd a = 1;
    Fxd b = 2;

    CHECK(nextafter(a, b) == a + eps);
    CHECK(nextafter(b, a) == b - eps);
    CHECK(nextafter(a, a) == a);
}


TEST_CASE("sqrt")
{
    using Fxd = fxd::fixed<8, 24>;

    CHECK(sqrt(Fxd{0}) == std::sqrt(0.0f));
    CHECK(sqrt(Fxd{1}) == std::sqrt(1.0f));

    CHECK(sqrt(Fxd{4}) == std::sqrt(4.0f));
    CHECK(sqrt(Fxd{9}) == std::sqrt(9.0f));
    CHECK(sqrt(Fxd{2}) == std::sqrt(2.0f));

    auto s3f_a = std::sqrt(3.0f);
    CHECK(s3f_a * s3f_a < 3.0f);
    std::fesetround(FE_UPWARD);
    auto s3f_b = 3.0f / s3f_a;
    std::fesetround(FE_TOWARDZERO);
    CHECK(s3f_b > s3f_a);
    CHECK(s3f_a * s3f_b <= 3.0f);

    auto s3_a = sqrt(Fxd{3});
    CHECK(s3_a * s3_a <= 3);

    using Fxd2 = fxd::ufixed<9, 23>;
    Fxd2 pi_a = std::numbers::pi_v<Fxd2>;
    Fxd2 s_pi_a = sqrt(pi_a);

    float pi_b = static_cast<float>(pi_a);
    float s_pi_b = std::sqrt(pi_b);
    CHECK(static_cast<float>(s_pi_a) == s_pi_b);

    auto r = fxd::sqrt(Fxd{-1});
    auto e = errno;
    CHECK(r == 0);
    CHECK(e == EDOM);
}


TEST_CASE("ilogb")
{
    using Fxd = fxd::fixed<16, 16>;

    CHECK(ilogb(Fxd{0.25}) == -2);
    CHECK(ilogb(Fxd{0.5}) == -1);
    CHECK(ilogb(Fxd{1.0}) == 0);
    CHECK(ilogb(Fxd{1.1}) == 0);
    CHECK(ilogb(Fxd{1.9999}) == 0);
    CHECK(ilogb(Fxd{2.0}) == 1);
    CHECK(ilogb(Fxd{2.9999}) == 1);
    CHECK(ilogb(Fxd{3.0}) == 1);
    CHECK(ilogb(Fxd{3.999}) == 1);
    CHECK(ilogb(Fxd{4.0}) == 2);
    CHECK(ilogb(Fxd{4.0}) == 2);
    CHECK(ilogb(Fxd{5.0}) == 2);
    CHECK(ilogb(Fxd{7.999}) == 2);
    CHECK(ilogb(Fxd{8.0}) == 3);
    CHECK(ilogb(Fxd{15.9999}) == 3);
    CHECK(ilogb(Fxd{16}) == 4);
    CHECK(ilogb(Fxd{255}) == 7);
    CHECK(ilogb(Fxd{256}) == 8);
    CHECK(ilogb(Fxd{-256}) == 8);
    CHECK(ilogb(Fxd{32767}) == 14);
    CHECK(ilogb(Fxd{-32767}) == 14);
    CHECK(ilogb(Fxd{-32768}) == 15);

    using UFxd = fxd::ufixed<16, 16>;

    CHECK(ilogb(UFxd{0.25}) == -2);
    CHECK(ilogb(UFxd{0.5}) == -1);
    CHECK(ilogb(UFxd{1.0}) == 0);
    CHECK(ilogb(UFxd{1.1}) == 0);
    CHECK(ilogb(UFxd{1.9999}) == 0);
    CHECK(ilogb(UFxd{2.0}) == 1);
    CHECK(ilogb(UFxd{2.9999}) == 1);
    CHECK(ilogb(UFxd{3.0}) == 1);
    CHECK(ilogb(UFxd{3.999}) == 1);
    CHECK(ilogb(UFxd{4.0}) == 2);
    CHECK(ilogb(UFxd{4.0}) == 2);
    CHECK(ilogb(UFxd{5.0}) == 2);
    CHECK(ilogb(UFxd{7.999}) == 2);
    CHECK(ilogb(UFxd{8.0}) == 3);
    CHECK(ilogb(UFxd{15.9999}) == 3);
    CHECK(ilogb(UFxd{16}) == 4);
    CHECK(ilogb(UFxd{255}) == 7);
    CHECK(ilogb(UFxd{256}) == 8);
    CHECK(ilogb(UFxd{32767}) == 14);
    CHECK(ilogb(UFxd{32768}) == 15);
    CHECK(ilogb(UFxd{65535}) == 15);

}
