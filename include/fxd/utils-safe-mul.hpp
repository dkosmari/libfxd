#ifndef LIBFXD_INCLUDING_UTILS_SAFE_HPP_IMPL
#error "Do not include this header directly. Either include 'saturate.hpp' or 'except.hpp'."
#endif

namespace round {

    namespace zero {
        // multiplication when no fractional bits
        template<fixed_point Fxd>
        requires (Fxd::frac_bits <= 0)
            constexpr
            Fxd
            mul(Fxd a,
                Fxd b)
        {
            constexpr int w = type_width<typename Fxd::raw_type>;
            // offset used for shifting left.
            constexpr int offset = w - Fxd::frac_bits;
            const auto c = utils::mul::mul<Fxd::bits>(a.raw_value, b.raw_value);

            // no lower bits will be discarded, so no rounding needed
            const auto d = utils::shift::shl_real(c, offset);

            // unshift, check that the value matches
            if (offset > 0 && c != utils::shift::shr_real(d, offset))
                return handler<Fxd>(utils::tuple::is_negative(c)
                                    ? error::underflow
                                    : error::overflow);

            return from_raw<Fxd>(utils::tuple::last(d));
        }



        // multiplication when there are fractional bits
        template<fixed_point Fxd>
        requires (Fxd::frac_bits > 0)
            constexpr
            Fxd
            mul(Fxd a,
                Fxd b)
        {
            using utils::tuple::is_negative;

            constexpr int w = type_width<typename Fxd::raw_type>;
            // offset used for shifting left.
            constexpr int offset = w - Fxd::frac_bits;

            auto c = utils::mul::mul<Fxd::bits>(a.raw_value, b.raw_value);

            if (is_negative(c)) {
                // negative numbers need a bias to zero when shifting
                const auto bias = utils::shift::make_bias_for(Fxd::frac_bits, c);
                c = utils::add::add(c, bias);
            }

            const auto d = utils::shift::shl(c, offset);
            // unshift, check that the value matches
            if (offset > 0 && c != utils::shift::shr(d, offset))
                return handler<Fxd>(is_negative(c)
                                    ? error::underflow
                                    : error::overflow);

            return from_raw<Fxd>(utils::tuple::last(d));
        }


    } // namespace zero


    namespace up {

        template<fixed_point Fxd>
        constexpr
        Fxd
        mul(Fxd a,
            Fxd b)
        {
            using utils::tuple::last;

            constexpr int w = type_width<typename Fxd::raw_type>;

            // offset used for shifting left
            constexpr int offset = w - Fxd::frac_bits;

            const auto c = utils::mul::mul<Fxd::bits>(a.raw_value, b.raw_value);

            if constexpr (Fxd::frac_bits <= 0) {
                // offset >= w

                const auto [d, ovf] = utils::shift::overflow::shl_real(c, offset);
                if (ovf)
                    return handler<Fxd>(utils::tuple::is_negative(c)
                                        ? error::underflow
                                        : error::overflow);

                return from_raw<Fxd>(last(d));

            } else {
                // offset < w

                const auto bias = utils::shift::make_bias_for(Fxd::frac_bits, c);
                const auto [biased_c, ovf] = utils::add::overflow::add(c, bias);
                if (ovf)
                    return handler<Fxd>(error::overflow);

                const auto [d, ovf2] = utils::shift::overflow::shl(biased_c, offset);
                if (offset > 0 && ovf2)
                    return handler<Fxd>(utils::tuple::is_negative(c)
                                        ? error::underflow
                                        : error::overflow);

                return from_raw<Fxd>(last(d));
            }

        }


    } // namespace up



    namespace down {

        template<fixed_point Fxd>
        ALWAYS_INLINE
        constexpr
        Fxd
        mul(Fxd a,
            Fxd b)
        {
            constexpr int w = type_width<typename Fxd::raw_type>;

            // offset used for shifting left
            constexpr int offset = w - Fxd::frac_bits;

            const auto c = utils::mul::mul<Fxd::bits>(a.raw_value, b.raw_value);

            const auto [d, ovf] = utils::shift::overflow::shl(c, offset);
            if (offset > 0 && ovf)
                return handler<Fxd>(utils::tuple::is_negative(c)
                                    ? error::underflow
                                    : error::overflow);

            return from_raw<Fxd>(utils::tuple::last(d));
        }

    } // namespace down




} // namespace round
