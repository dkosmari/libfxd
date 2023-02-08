#include <cmath>

#include <fxd/limits.hpp>
#include <fxd/math.hpp>


#include "setup.hpp"


TEST_CASE("abs")
{
    using Fxd = fxd::fixed<8, 10>;

    Fxd a = 3;
    CHECK(abs(a) == 3);

    Fxd b = -3;
    CHECK(abs(b) == 3);
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
