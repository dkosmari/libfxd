#include <climits>
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

    CHECK(std::is_same_v<typename Fxd::float_type, Flt>);
    CHECK(LimFxd::is_specialized);
    CHECK(LimFxd::radix == LimFlt::radix);
    CHECK(LimFxd::digits == LimFlt::digits);
    CHECK(LimFxd::digits10 == LimFlt::digits10);
    CHECK(LimFxd::max_digits10 == LimFlt::max_digits10);
}


TEST_CASE("limits-special")
{
    CHECK(std::is_same_v<typename fxd:: fixed<1, 25>::float_type, double>);
    CHECK(std::is_same_v<typename fxd::ufixed<1, 24>::float_type, double>);

    CHECK(std::is_same_v<typename fxd:: fixed<1, 54>::float_type, long double>);
    CHECK(std::is_same_v<typename fxd::ufixed<0, 54>::float_type, long double>);
}
