#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <type_traits>
#include <tuple>

#include "catch2/catch_amalgamated.hpp"

#include <fxd/fxd.hpp>

#include "setup.hpp"


using equiv_types = std::tuple<

#if LDBL_MANT_DIG == 64
    std::tuple<fxd::ufixed<0, 64>, long double>,
#endif

    std::tuple<fxd:: fixed<1, 24>, float>,
    std::tuple<fxd::ufixed<0, 24>, float>,

    std::tuple<fxd:: fixed<1, 53>, double>,
    std::tuple<fxd::ufixed<0, 53>, double>

>;


TEMPLATE_LIST_TEST_CASE("limits",
                        "",
                        equiv_types)
{
    using Fxd = std::tuple_element_t<0, TestType>;
    using Flt = std::tuple_element_t<1, TestType>;

    using LimFxd = std::numeric_limits<Fxd>;
    using LimFlt = std::numeric_limits<Flt>;

    CHECK(std::is_same_v<typename LimFxd::float_type, Flt>);
    CHECK(LimFxd::is_specialized);
    CHECK(LimFxd::radix == LimFlt::radix);
    CHECK(LimFxd::digits == LimFlt::digits);
    CHECK(LimFxd::digits10 == LimFlt::digits10);
    CHECK(LimFxd::max_digits10 == LimFlt::max_digits10);

    using fxd::except::make_fixed;
    using fxd::except::from_raw;

    Fxd lowest = LimFxd::lowest();
    Fxd max = LimFxd::max();
    CAPTURE(lowest);
    CAPTURE(max);

    CHECK_NOTHROW(from_raw<Fxd>(lowest.raw_value));
    CHECK_NOTHROW(from_raw<Fxd>(max.raw_value));

    Flt too_low = std::nextafter(static_cast<Flt>(LimFxd::lowest()),
                                 -LimFlt::infinity());
    CHECK_THROWS_AS(make_fixed<Fxd>(too_low), std::underflow_error);

    Flt too_high = std::nextafter(static_cast<Flt>(LimFxd::max()),
                                  LimFlt::infinity());
    CHECK_THROWS_AS(make_fixed<Fxd>(too_high), std::overflow_error);
}


template<fxd::fixed_point Fxd>
using float_for = typename std::numeric_limits<Fxd>::float_type;

TEST_CASE("limits-float_type")
{
    CHECK(std::is_same_v<float_for<fxd:: fixed<1, 25>>, double>);
    CHECK(std::is_same_v<float_for<fxd::ufixed<1, 24>>, double>);

    CHECK(std::is_same_v<float_for<fxd:: fixed<1, 54>>, long double>);
    CHECK(std::is_same_v<float_for<fxd::ufixed<0, 54>>, long double>);
}
