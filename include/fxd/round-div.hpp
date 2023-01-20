#ifndef LIBFXD_ROUND_DIV_HPP
#define LIBFXD_ROUND_DIV_HPP

#include <csignal>

// #include <iostream>

#include "fixed.hpp"

#include "concepts.hpp"
#include "utils-div.hpp"
#include "utils-shift.hpp"
#include "utils.hpp"


namespace fxd::round {

    namespace zero {

        template<fixed_point Fxd>
        ALWAYS_INLINE
        constexpr
        Fxd
        div(Fxd a,
            Fxd b)
            noexcept
        {
            const auto r = utils::div::div<Fxd::frac_bits>(a.raw_value,
                                                           b.raw_value);
            if (!r)
                std::raise(SIGFPE);

            auto [c, expo, rem] = *r;

            // offset used for shifting left
            const int offset = expo + Fxd::frac_bits;

            // Right-shifting a negative number would round it down, not to zero.
            if (c < 0 && offset < 0)
                c += utils::shift::make_bias_for(-offset, c);

            const auto d = utils::shift::shl(c, offset);

            return Fxd::from_raw(d);
        }

    } // namespace zero



    namespace up {

        template<fixed_point Fxd>
        ALWAYS_INLINE
        constexpr
        Fxd
        div(Fxd a,
            Fxd b)
            noexcept
        {
            const auto r = utils::div::div<Fxd::frac_bits>(a.raw_value,
                                                           b.raw_value);
            if (!r)
                std::raise(SIGFPE);

            auto [c, expo, rem] = *r;

            const bool neg = (a.raw_value < 0) != (b.raw_value < 0);

            // offset used for shifting left
            const int offset = expo + Fxd::frac_bits;

            // When a/b is positive, it may have been rounded down.
            if (!neg && rem)
                ++c;

            // Right-shifting will always round down.
            if (offset < 0)
                c += utils::shift::make_bias_for(-offset, c);

            const auto d = utils::shift::shl(c, offset);

            return Fxd::from_raw(d);
        }

    } // namespace up



    namespace down {

        template<fixed_point Fxd>
        ALWAYS_INLINE
        constexpr
        Fxd
        div(Fxd a,
            Fxd b)
            noexcept
        {
            const auto r = utils::div::div<Fxd::frac_bits>(a.raw_value,
                                                           b.raw_value);
            if (!r)
                std::raise(SIGFPE);

            auto [c, expo, rem] = *r;

            const bool neg = (a.raw_value < 0) != (b.raw_value < 0);

            // offset used for shifting left
            const int offset = expo + Fxd::frac_bits;

             // When a/b is negative, it may have been rounded up.
            if (neg && rem)
                --c;

            // Right-shifting always rounds down, so we don't need top handle it.

            const auto d = utils::shift::shl(c, offset);

            return Fxd::from_raw(d);
        }

    } // namespace down


}


#endif
