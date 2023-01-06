#ifndef LIBFXD_SAFE_OPERATORS_HPP
#define LIBFXD_SAFE_OPERATORS_HPP

#include <concepts>
#include <cstdlib>
#include <stdexcept>
#include <utility> // in_range()

#include "fixed.hpp"
#include "concepts.hpp"
#include "limits.hpp"
#include "traits.hpp"


namespace fxd::safe {



    enum class error {
        underflow,
        overflow,
        not_a_number
    };



    template<typename Handler>
    struct safe_operators {



       template<fxd::fixed_point Fxd>
        static constexpr bool is_noexcept =
           noexcept(Handler::template handle<Fxd>(error::overflow));



        template<fxd::fixed_point Fxd>
        static constexpr
        Fxd
        handler(error e)
            noexcept(is_noexcept<Fxd>)
        {
            return Handler::template handle<Fxd>(e);
        }



        template<fxd::fixed_point Fxd,
                 std::integral I>
        static constexpr
        Fxd
        from_raw(I val)
            noexcept(is_noexcept<Fxd>)
        {
            using Lim = std::numeric_limits<Fxd>;

            if (std::cmp_less(val, Lim::lowest().raw_value))
                return handler<Fxd>(error::underflow);

            if (std::cmp_greater(val, Lim::max().raw_value))
                return handler<Fxd>(error::overflow);

            return Fxd::from_raw(val);
        }



        template<fxd::fixed_point Fxd,
                 std::integral I>
        static constexpr
        Fxd
        make_fixed(I val)
            noexcept(is_noexcept<Fxd>)
        {
            if constexpr (Fxd::frac_bits < 0) {

                return from_raw<Fxd>(utils::shrz(val, -Fxd::frac_bits));

            } else {

                using Raw = typename Fxd::raw_type;
                using Lim = std::numeric_limits<Raw>;
                constexpr int w = utils::type_width<Raw>;

                // ensure val can be represented as raw_type
                if (std::cmp_less(val, Lim::min()))
                    return handler<Fxd>(error::underflow);
                if (std::cmp_greater(val, Lim::max()))
                    return handler<Fxd>(error::overflow);

                const Raw d = utils::shr<Raw>(val, w - Fxd::frac_bits);
                const Raw rval = utils::shl<Raw>(val, Fxd::frac_bits);

                if (d > 0)
                    return handler<Fxd>(error::overflow);

                if (std::cmp_less(d, -1))
                    return handler<Fxd>(error::underflow);

                if ((rval < 0) != (d < 0))
                    return handler<Fxd>(d < 0 ? error::underflow : error::overflow);

                return from_raw<Fxd>(rval);

            }
        }



        template<fxd::fixed_point Fxd,
                 std::floating_point Flt>
        static
        Fxd
        make_fixed(Flt val)
            noexcept(is_noexcept<Fxd>)
        {
            using Lim = std::numeric_limits<Fxd>;

            if (!std::isfinite(val)) {
                if (std::isnan(val))
                    return handler<Fxd>(error::not_a_number);
                return handler<Fxd>(val < 0 ? error::underflow : error::overflow);
            }

            if (val < static_cast<Flt>(Lim::lowest()))
                return handler<Fxd>(error::underflow);

            if (val > static_cast<Flt>(Lim::max()))
                return handler<Fxd>(error::overflow);

            return Fxd{val};
        }



        template<int Int,
                 int Frac,
                 typename T = fxd::select_int_t<Int + Frac>,
                 std::convertible_to<fxd::fixed<Int, Frac, T>> Src>
        static constexpr
        fxd::fixed<Int, Frac, T>
        make_fixed(Src src)
            noexcept(is_noexcept<fxd::fixed<Int, Frac, T>>)
        {
            return make_fixed<fxd::fixed<Int, Frac, T>>(src);
        }



        template<int Int,
                 int Frac,
                 typename T = fxd::select_uint_t<Int + Frac>,
                 std::convertible_to<fxd::fixed<Int, Frac, T>> Src>
        static constexpr
        fxd::fixed<Int, Frac, T>
        make_ufixed(Src src)
            noexcept(is_noexcept<fxd::fixed<Int, Frac, T>>)
        {
            return make_fixed<fxd::fixed<Int, Frac, T>>(src);
        }



        template<fixed_point Fxd,
                 std::convertible_to<Fxd> Src>
        static constexpr
        Fxd&
        assign(Fxd& dst,
               Src src)
            noexcept(is_noexcept<Fxd>)
        {
            return dst = make_fixed<Fxd>(src);
        }



        template<fixed_point Fxd>
        static constexpr
        Fxd&
        inc(Fxd& f)
            noexcept(is_noexcept<Fxd>)
        {
            // if no integral bit, it always overflows
            if constexpr (Fxd::int_bits < 1)
                return f = handler<Fxd>(error::overflow);

            // if not units bit, value never changes
            if constexpr (Fxd::frac_bits < 0)
                return f;

            auto [result, carry] = utils::add_overflow(f.raw_value, Fxd::one.raw_value);
            if (carry)
                return f = handler<Fxd>(error::overflow);

            f.raw_value = result;

            if (f.raw_value != result)
                return f = handler<Fxd>(error::overflow);

            return f;
        }



        template<fixed_point Fxd>
        static constexpr
        Fxd
        post_inc(Fxd& f)
            noexcept(is_noexcept<Fxd>)
        {
            Fxd old = f;
            inc(f);
            return old;
        }



        template<fixed_point Fxd>
        static constexpr
        Fxd&
        dec(Fxd& f)
            noexcept(is_noexcept<Fxd>)
        {
            // if no integral bit, it always underflows
            if constexpr (Fxd::int_bits < 1)
                return f = handler<Fxd>(error::underflow);

            // if not units bit, value never changes
            if constexpr (Fxd::frac_bits < 0)
                return f;

            using Raw = Fxd::raw_type;
            constexpr Raw one = Fxd::one.raw_value;

            auto [result, overflow] = utils::sub_overflow(f.raw_value, one);
            if (overflow)
                return f = handler<Fxd>(error::underflow);

            f.raw_value = result;

            if (f.raw_value != result)
                return f = handler<Fxd>(error::underflow);

            return f;
        }



        template<fixed_point Fxd>
        static constexpr
        Fxd
        post_dec(Fxd& f)
            noexcept(is_noexcept<Fxd>)
        {
            Fxd old = f;
            dec(f);
            return old;
        }



        template<fixed_point Fxd>
        static constexpr
        Fxd
        negate(const Fxd& f)
            noexcept(is_noexcept<Fxd>)
        {
            using Lim = std::numeric_limits<Fxd>;

            if constexpr (!Lim::is_signed)
                return f.raw_value == 0 ? f : handler<Fxd>(error::underflow);

            if (f == Lim::lowest())
                return handler<Fxd>(error::overflow);

            return -f;
        }



        template<fixed_point Fxd>
        static constexpr
        Fxd
        plus(Fxd a,
             Fxd b)
            noexcept(is_noexcept<Fxd>)
        {
            auto [result, overflow] = utils::add_overflow(a.raw_value, b.raw_value);
            if (overflow) {
                if constexpr (std::numeric_limits<Fxd>::is_signed)
                    return handler<Fxd>(a.raw_value < 0 ? error::underflow : error::overflow);
                else
                    return handler<Fxd>(error::overflow);
            }

            return from_raw<Fxd>(result);
        }



        template<fixed_point Fxd>
        static constexpr
        Fxd
        minus(Fxd a,
              Fxd b)
            noexcept(is_noexcept<Fxd>)
        {
            auto [result, overflow] = utils::sub_overflow(a.raw_value, b.raw_value);
            if (overflow) {
                if constexpr (std::numeric_limits<Fxd>::is_signed)
                    return handler<Fxd>(a.raw_value < 0 ? error::underflow : error::overflow);
                else
                    return handler<Fxd>(error::underflow);
            }

            return from_raw<Fxd>(result);
        }




        template<fixed_point Fxd>
        static constexpr
        Fxd
        multiplies(Fxd a,
                   Fxd b)
            noexcept(is_noexcept<Fxd>)
        {
            using R = typename Fxd::raw_type;

            if constexpr (has_wider_v<R>) {

                using W = wider_t<R>;

                const W aa = a.raw_value;
                const W bb = b.raw_value;

                const W cc = aa * bb;
                const W dd = utils::shrz<W>(cc, Fxd::frac_bits);

                if constexpr (Fxd::frac_bits < 0)
                    /*
                     * If frac_bits < 0, it was actually a left shift,
                     * so significant high bits might have been lost.
                     * Here we unshift dd to see if the value still matches.
                     */
                    if (cc != utils::shlz<W>(dd, Fxd::frac_bits))
                        return handler<Fxd>(cc < 0 ? error::underflow : error::overflow);

                return from_raw<Fxd>(dd);

            } else {

                const auto c = utils::full_mult(a.raw_value,
                                                b.raw_value);

                const auto d = utils::shrz(c, Fxd::frac_bits);

                if constexpr (Fxd::frac_bits < 0)
                    /*
                     * If frac_bits < 0, it was actually a left shift,
                     * so significant high bits might have been lost.
                     * Here we unshift d to see if the value still matches.
                     */
                    if (c != utils::shlz(d, Fxd::frac_bits))
                        return handler<Fxd>(c.second < 0 ? error::underflow : error::overflow);

                const R dd = static_cast<R>(d.first);

                if constexpr (std::numeric_limits<R>::is_signed) {
                    // only valid values for d.second is 0 or -1
                    if (d.second < -1)
                        return handler<Fxd>(error::underflow);
                    if (d.second > 0)
                        return handler<Fxd>(error::overflow);
                    // d.second is just a sign extension of the top bit of dd
                    // if they disagree in sign, there's underflow or overflow
                    if ((d.second < 0) != (dd < 0))
                        return handler<Fxd>(d.second < 0 ? error::underflow : error::overflow);

                    return from_raw<Fxd>(dd);
                } else {
                    if (d.second)
                        return handler<Fxd>(error::overflow);
                    return from_raw<Fxd>(d.first);
                }

            }

        }



        template<fixed_point Fxd>
        static constexpr
        Fxd
        divides(Fxd a,
                Fxd b)
            noexcept(is_noexcept<Fxd>)
        {
            using R = typename Fxd::raw_type;

            if (!b) {
                if (!a)
                    return handler<Fxd>(error::not_a_number);
                return handler<Fxd>(a < 0 ? error::underflow : error::overflow);
            }

            if constexpr (has_wider_v<R>) {
                using W = wider_t<R>;
                const W aa = utils::shl<W>(a.raw_value, Fxd::frac_bits);
                const W bb = b.raw_value;

                if constexpr (std::numeric_limits<W>::is_signed) {
                    if (aa == std::numeric_limits<W>::lowest()
                        && bb ==  -1)
                        return handler<Fxd>(error::overflow);
                }

                const W cc = aa / bb;
                return from_raw<Fxd>(cc);
            } else {
                const bool neg_a = a.raw_value < 0;
                const bool neg_b = b.raw_value < 0;

                using U = std::make_unsigned_t<R>;
                U ua = a.raw_value;
                U ub = b.raw_value;

                if (neg_a)
                    ua = -ua;

                if (neg_b)
                    ub = -ub;

                auto q = utils::full_div<Fxd::frac_bits>(ua, ub);
                auto r = utils::shrz(q, utils::type_width<U> - Fxd::frac_bits);
                // TODO: handle extra bits like the unsafe version

                if (neg_a != neg_b) {
                    // must negate the result
                    if (r.second // high bits are nonzero
                        || std::cmp_greater(r.first, std::numeric_limits<R>::max())) // too large
                        return handler<Fxd>(error::underflow);
                    return from_raw<Fxd>(-static_cast<R>(r.first));
                } else {
                    if (r.second)
                        return handler<Fxd>(error::overflow);
                    return from_raw<Fxd>(r.first);
                }
            }
        }



        template<fixed_point Fxd>
        static constexpr
        Fxd
        abs(Fxd f)
            noexcept(is_noexcept<Fxd>)
        {
            if (f.raw_value == std::numeric_limits<typename Fxd::raw_type>::min())
                return handle<Fxd>(error::overflow);
            return from_raw(std::abs(f.raw_value));
        }



        template<fixed_point Fxd>
        static constexpr
        Fxd
        fdim(Fxd a, Fxd b)
            noexcept(is_noexcept<Fxd>)
        {
            if(a > b)
                return minus(a, b);
            else
                return 0;
        }



        template<fixed_point Fxd>
        static constexpr
        Fxd
        nextafter(Fxd from, Fxd to)
            noexcept(is_noexcept<Fxd>)
        {
            constexpr Fxd e = std::numeric_limits<Fxd>::epsilon();
            if (from < to)
                return plus(from, e);
            if (from > to)
                return minus(from, e);
            return to;
        }


    };



    struct saturate_handler {
        template<fxd::fixed_point Fxd>
        [[nodiscard]]
        static
        Fxd
        handle(error e)
            noexcept
        {
            switch (e) {
                case error::underflow:
                    return std::numeric_limits<Fxd>::lowest();
                case error::overflow:
                    return std::numeric_limits<Fxd>::max();
                default:
                    std::abort();
            }
        }
    };


    using saturate = safe_operators<saturate_handler>;



    struct except_handler {
        template<fxd::fixed_point Fxd>
        [[noreturn]]
        static
        Fxd
        handle(error e)
        {
            switch (e) {
                case error::underflow:
                    throw std::underflow_error{"underflow"};
                case error::overflow:
                    throw std::overflow_error{"overflow"};
                case error::not_a_number:
                    throw std::invalid_argument{"not a number"};
                default:
                    std::abort();
            }
        }
    };


    using except = safe_operators<except_handler>;


}


#endif
