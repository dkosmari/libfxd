#ifndef LIBFXD_INCLUDING_UTILS_SAFE_HPP_IMPL
#error "Do not include this header directly. Either include 'saturate.hpp' or 'except.hpp'."
#endif


namespace round {

    namespace zero {

        template<fixed_point Fxd>
        constexpr
        Fxd
        div(Fxd a,
            Fxd b)
        {
            const auto r = utils::div::div<Fxd::frac_bits, true>(a.raw_value,
                                                                 b.raw_value);

            if (!r)
                return handler<Fxd>(r.error());

            auto [c, expo, rem] = *r;

            const int offset = expo + Fxd::frac_bits;

            if (c < 0 && offset < 0)
                c += utils::shift::make_bias_for(-offset, c);

            const auto [d, ovf] = utils::shift::overflow::shl(c, offset);

            if (offset > 0 && ovf)
                return handler<Fxd>(c < 0
                                    ? error::underflow
                                    : error::overflow);

            return from_raw<Fxd>(d);
        }

    } // namespace zero


    namespace up {

        template<fixed_point Fxd>
        constexpr
        Fxd
        div(Fxd a,
            Fxd b)
        {
            const auto r = utils::div::div<Fxd::frac_bits, true>(a.raw_value,
                                                                 b.raw_value);
            if (!r)
                return handler<Fxd>(r.error());

            auto [c, expo, rem] = *r;

            // Note: div() rounds to zero; so we might lose the info about which
            // side of zero the result was.
            const bool neg = (a.raw_value < 0) != (b.raw_value < 0);

            // offset used for shifting left
            const int offset = expo + Fxd::frac_bits;

            // When a/b is positive, it may have been rounded down.
            if (!neg && rem) {
                if (c == std::numeric_limits<decltype(c)>::max())
                    return handler<Fxd>(error::overflow);
                ++c;
            }

            // Right-shifting will always round down.
            if (offset < 0) {
                const auto bias = utils::shift::make_bias_for(-offset, c);
                const auto [biased_c, ovf] = utils::add::overflow::add(c, bias);
                if (ovf)
                    return handler<Fxd>(error::overflow);
                c = biased_c;
            }

            const auto [d, ovf] = utils::shift::overflow::shl(c, offset);
            if (offset > 0 && ovf)
                return handler<Fxd>(neg ? error::underflow : error::overflow);

            return from_raw<Fxd>(d);
        }

    } // namespace up



    namespace down {

        template<fixed_point Fxd>
        constexpr
        Fxd
        div(Fxd a,
            Fxd b)
        {
            const auto r = utils::div::div<Fxd::frac_bits, true>(a.raw_value,
                                                                 b.raw_value);
            if (!r)
                return handler<Fxd>(r.error());

            auto [c, expo, rem] = *r;

            const bool neg = (a.raw_value < 0) != (b.raw_value < 0);

            // offset used for shifting left
            const int offset = expo + Fxd::frac_bits;

            // When a/b is negative, it may have been rounded up.
            if (neg && rem) {
                if (c == std::numeric_limits<decltype(c)>::min())
                    return handler<Fxd>(error::underflow);
                --c;
            }


            // Right-shifting always rounds down, so we don't need to bias it.

            const auto [d, ovf] = utils::shift::overflow::shl(c, offset);
            if (offset > 0 && ovf)
                return handler<Fxd>(neg ? error::underflow : error::overflow);

            return from_raw<Fxd>(d);
        }

    } // namespace down



} // namespace round