#ifndef TEST_TYPES_HPP
#define TEST_TYPES_HPP

#include <cfloat>
#include <tuple>


using test_types = std::tuple<

#if FLT_MANT_DIG >= 24

    fxd::fixed<13, 12>,
    fxd::fixed<1, 24>,
    fxd::fixed<0, 25>,
    fxd::fixed<-1, 26>,
    fxd::fixed<24, 1>,
    fxd::fixed<25, 0>,
    fxd::fixed<26, -1>,

    fxd::ufixed<12, 12>,
    fxd::ufixed<1, 23>,
    fxd::ufixed<0, 24>,
    fxd::ufixed<-1, 25>,
    fxd::ufixed<23, 1>,
    fxd::ufixed<24, 0>,
    fxd::ufixed<25, -1>,

#endif

#if DBL_MANT_DIG >= 53

    fxd::fixed<34, 20>,
    fxd::fixed<53, 1>,
    fxd::fixed<54, 0>,
    fxd::fixed<55, -1>,
    fxd::fixed<1, 53>,
    fxd::fixed<0, 54>,
    fxd::fixed<-1, 55>,

    fxd::ufixed<21, 32>,
    fxd::ufixed<52, 1>,
    fxd::ufixed<53, 0>,
    fxd::ufixed<54, -1>,
    fxd::ufixed<1, 52>,
    fxd::ufixed<0, 53>,
    fxd::ufixed<-1, 54>,

#endif

#if LDBL_MANT_DIG >= 64

    fxd::fixed<32, 32>,
    fxd::fixed<63, 1>,
    fxd::fixed<64, 0>,
    fxd::fixed<65, -1>,
    fxd::fixed<1, 63>,
    fxd::fixed<0, 64>,
    fxd::fixed<-1, 65>,

    fxd::ufixed<32, 32>,
    fxd::ufixed<63, 1>,
    fxd::ufixed<64, 0>,
    fxd::ufixed<65, -1>,
    fxd::ufixed<1, 63>,
    fxd::ufixed<0, 64>,
    fxd::ufixed<-1, 65>

#else
#warning "'long double' doesnt have 64 bits of mantissa."
    fxd::fixed<8, 8>

#endif

    >;




#endif
