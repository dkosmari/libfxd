#ifndef LIBFXD_ROUND_MUL_HPP
#define LIBFXD_ROUND_MUL_HPP

// #include <iostream>
// #include <iomanip>

#include "fixed.hpp"

#include "concepts.hpp"
#include "utils-add.hpp"
#include "utils-mul.hpp"
#include "utils-shift.hpp"
#include "utils.hpp"


namespace fxd::round {


    namespace zero {

        template<fixed_point Fxd>
        ALWAYS_INLINE
        constexpr
        Fxd
        mul(Fxd a,
            Fxd b)
            noexcept
        {

            using utils::tuple::last;
            using utils::shift::shl;

            constexpr int w = type_width<typename Fxd::raw_type>;

            // offset used for shifting left
            constexpr int offset = w - Fxd::frac_bits;

            const auto c = utils::mul::mul<Fxd::bits>(a.raw_value, b.raw_value);

            if constexpr (Fxd::frac_bits <= 0) {

                // no lower bits will be lost, no rounding is needed
                const auto d = shl(c, offset);
                return Fxd::from_raw(last(d));

            } else {

                // will lose lower bits
                if (utils::tuple::is_negative(c)) {

                    // negative numbers need a bias to zero to round up
                    const auto bias = utils::shift::make_bias_for(Fxd::frac_bits, c);
                    const auto biased_c = utils::add::add(c, bias);
                    const auto d = shl(biased_c, offset);
                    return Fxd::from_raw(last(d));

                } else {

                    const auto d = shl(c, offset);
                    return Fxd::from_raw(last(d));

                }

            }

        }

    } // namespace zero



    namespace up {

        template<fixed_point Fxd>
        ALWAYS_INLINE
        constexpr
        Fxd
        mul(Fxd a,
            Fxd b)
            noexcept
        {
            using utils::tuple::last;
            using utils::shift::shl;

            // using std::clog;
            // using std::endl;

            constexpr int w = type_width<typename Fxd::raw_type>;

            // offset used for shifting left
            constexpr int offset = w - Fxd::frac_bits;

            const auto c = utils::mul::mul<Fxd::bits>(a.raw_value, b.raw_value);

            if constexpr (Fxd::frac_bits <= 0) {

                // clog << "::: mul path A" << endl;
                // no lower bits will be lost, no rounding is needed
                const auto d = shl(c, offset);
                return Fxd::from_raw(last(d));

            } else {

                // clog << "::: mul path B" << endl;
                // clog << "offset = " << offset << endl;
                // lower bits may be lost, so it needs a bias
                const auto bias = utils::shift::make_bias_for(Fxd::frac_bits, c);
                const auto biased_c = utils::add::add(c, bias);
                const auto d = shl(biased_c, offset);
                // if constexpr (std::integral<decltype(c)>) {
                //     clog << "c = 0x" << std::hex << c << endl;
                //     clog << "bias = 0x" << bias << endl;
                //     clog << "biased_c = 0x" << biased_c << endl;
                //     clog << "d = 0x" << d << endl;
                // }

                return Fxd::from_raw(last(d));

            }

        }

    } // namespace up



    namespace down {

        // rounding down is the default behavior of >>, so no special handling is needed

        template<fixed_point Fxd>
        ALWAYS_INLINE
        constexpr
        Fxd
        mul(Fxd a,
            Fxd b)
            noexcept
        {
            using utils::tuple::last;
            using utils::shift::shl;

            constexpr int w = type_width<typename Fxd::raw_type>;

            // offset used for shifting left
            constexpr int offset = w - Fxd::frac_bits;

            const auto c = utils::mul::mul<Fxd::bits>(a.raw_value, b.raw_value);

            const auto d = shl(c, offset);

            return Fxd::from_raw(last(d));
        }

    } // namespace down


} // namespace fxd::round


#endif
