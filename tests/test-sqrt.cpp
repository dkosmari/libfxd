#include <cfloat>
#include <stdexcept>
#include <tuple>

#include <fxd/limits.hpp>
#include <fxd/math.hpp>

#include "rng.hpp"
#include "setup.hpp"


const int max_iterations = 1000;


using sqrt_test_types = std::tuple<

#if FLT_MANT_DIG >= 24

    fxd::fixed<13, 12>,
    fxd::fixed<24, 1>,
    fxd::fixed<25, 0>,

    fxd::ufixed<12, 12>,
    fxd::ufixed<1, 23>,
    fxd::ufixed<23, 1>,
    fxd::ufixed<24, 0>,

#endif

#if DBL_MANT_DIG >= 53

    fxd::fixed<34, 20>,
    fxd::fixed<53, 1>,
    fxd::fixed<54, 0>,

    fxd::ufixed<21, 32>,
    fxd::ufixed<52, 1>,
    fxd::ufixed<53, 0>,
    fxd::ufixed<1, 52>,

#endif

#if LDBL_MANT_DIG >= 64

    fxd::fixed<32, 32>,
    fxd::fixed<63, 1>,
    fxd::fixed<64, 0>,

    fxd::ufixed<32, 32>,
    fxd::ufixed<63, 1>,
    fxd::ufixed<64, 0>,
    fxd::ufixed<1, 63>

#else
#warning "'long double' doesnt have 64 bits of mantissa."
    fxd::fixed<8, 8>

#endif

    >;



TEMPLATE_LIST_TEST_CASE("random",
                        "",
                        sqrt_test_types)
{
    using Fxd = TestType;

    constexpr Fxd eps = std::numeric_limits<Fxd>::epsilon();

    RNG<Fxd> rng{0};

    for (int i = 0; i < max_iterations; ++i) {
        Fxd x = rng.get();
        CAPTURE(x);

        Fxd y = fxd::sqrt(x);
        CAPTURE(y);

        try  {
            // ensure it's not larger than the actual square root
            Fxd yy = fxd::except::mul(y, y);
            CHECK(yy <= x);

            // ensure the very next larger value exceeds the square root
            Fxd ye = y + eps;
            Fxd yye = fxd::except::up::mul(ye, ye);
            CHECK(yye > x);
        }
        catch (std::overflow_error&) {
            continue;
        }

    }

}
