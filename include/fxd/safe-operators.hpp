#ifndef LIBFXD_SAFE_OPERATORS_HPP
#define LIBFXD_SAFE_OPERATORS_HPP

#include <concepts>
#include <csignal>
#include <cstdlib>
#include <stdexcept>
#include <utility> // in_range(), forward()

#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <cxxabi.h>

#include "fixed.hpp"
#include "concepts.hpp"
#include "limits.hpp"
#include "traits.hpp"


namespace fxd::safe {


    template<typename T>
    std::string
    type_name()
    {
        char* p = abi::__cxa_demangle(typeid(T).name(), 0, 0, 0);
        std::string result = p;
        std::free(p);
        return result;
    }


    enum class error {
        underflow,
        overflow,
        div_by_zero
    };


    // error handler must have signature:
    //     fxd::fixed<...> handler(error e);
    template<typename H>
    concept error_handler =
        std::is_invocable_v<H, error>
        && fxd::is_fixed_point_v<std::invoke_result_t<H, error>>;




    template<fxd::fixed_point Fxd>
    [[noreturn]]
    Fxd
    except(error e)
    {
        switch (e) {
            case error::underflow:
                throw std::underflow_error{"fixed point underflow"};
            case error::overflow:
                throw std::overflow_error{"fixed point overflow"};
            case error::div_by_zero:
                throw std::domain_error{"fixed point div by zero"};
            default:
                throw std::logic_error{"unknown error"};
        }
    }




#define IS_NOEXCEPT std::is_nothrow_invocable_v<H, error>


    template<fxd::fixed_point Fxd,
             typename T,
             error_handler H>
    constexpr
    Fxd
    from_raw(T x,
             H&& handler)
        noexcept(IS_NOEXCEPT)
    {
        using Lim = std::numeric_limits<Fxd>;

        if (std::cmp_less(x, Lim::lowest().raw_value))
            return handler(error::underflow);

        if (std::cmp_greater(x, Lim::max().raw_value))
            return handler(error::overflow);

        return Fxd::from_raw(x);
    }



    template<fxd::fixed_point Fxd,
             std::integral I,
             error_handler H>
    constexpr
    Fxd
    make_fixed(const I& src,
               H&& handler)
        noexcept(IS_NOEXCEPT)
    {
        using Lim = std::numeric_limits<Fxd>;
        using Raw = typename Fxd::raw_type;

        // TODO: review these

        if (std::cmp_less(src, static_cast<Raw>(Lim::lowest())))
            return handler(error::underflow);

        if (std::cmp_greater(src, static_cast<Raw>(Lim::max())))
            return handler(error::overflow);

        return Fxd{src};
    }



    template<fxd::fixed_point Fxd,
             std::floating_point Flt,
             error_handler H>
    constexpr
    Fxd
    make_fixed(const Flt& src,
               H&& handler)
        noexcept(IS_NOEXCEPT)
    {
        using Lim = std::numeric_limits<Fxd>;

        // TODO: check if rounding doesn't ruin the limit
        // when Fxd has more bits than Flt's mantissa.

        if (src < static_cast<Flt>(Lim::lowest()))
            return handler(error::underflow);

        if (src > static_cast<Flt>(Lim::max()))
            return handler(error::overflow);

        return Fxd{src};
    }



    template<int Int,
             int Frac,
             typename T = fxd::select_int_t<Int + Frac>,
             std::convertible_to<fxd::fixed<Int, Frac, T>> Src,
             error_handler H>
    constexpr
    fxd::fixed<Int, Frac, T>
    make_fixed(const Src& src,
               H&& handler)
        noexcept(IS_NOEXCEPT)
    {
        return make_fixed<fxd::fixed<Int, Frac, T>>(src,
                                                    std::forward<H>(handler));
    }



    template<int Int,
             int Frac,
             typename T = fxd::select_uint_t<Int + Frac>,
             std::convertible_to<fxd::fixed<Int, Frac, T>> Src,
             error_handler H>
    constexpr
    fxd::fixed<Int, Frac, T>
    make_ufixed(const Src& src,
                H&& handler)
        noexcept(IS_NOEXCEPT)
    {
        return make_fixed<fxd::fixed<Int, Frac, T>>(src,
                                                    std::forward<H>(handler));
    }



    template<fixed_point Fxd,
             std::convertible_to<Fxd> Src,
             error_handler H>
    constexpr
    Fxd&
    assign(Fxd& dst,
           Src src,
           H&& handler)
        noexcept(IS_NOEXCEPT)
    {
        return dst = make_fixed<Fxd>(src,
                                     std::forward<H>(handler));
    }



    // TODO: other assignment operators?





    // Increment/decrement


    template<fixed_point Fxd,
             error_handler H>
    constexpr
    Fxd&
    inc(Fxd& f,
        H&& handler)
        noexcept(IS_NOEXCEPT)
    {
        // if no integral bit, it always overflows
        if constexpr (Fxd::int_bits < 1)
            return f = handler(error::overflow);

        // if not units bit, value never changes
        if constexpr (Fxd::frac_bits < 0)
            return f;

        using Raw = Fxd::raw_type;
        Raw one = Fxd::one.raw_value;
        Raw result = 0;

        if (__builtin_add_overflow(f.raw_value, one, &result))
            return f = handler(error::overflow);

        f.raw_value = result;

        if (f.raw_value != result)
            return f = handler(error::overflow);

        return f;
    }



    template<fixed_point Fxd,
             error_handler H>
    constexpr
    Fxd
    post_inc(Fxd& f,
             H&& handler)
        noexcept(IS_NOEXCEPT)
    {
        Fxd old = f;
        inc(f, std::forward<H>(handler));
        return old;
    }



    template<fixed_point Fxd,
             error_handler H>
    constexpr
    Fxd&
    dec(Fxd& f,
        H&& handler)
        noexcept(IS_NOEXCEPT)
    {
        // if no integral bit, it always underflows
        if constexpr (Fxd::int_bits < 1)
            return f = handler(error::underflow);

        // if not units bit, value never changes
        if constexpr (Fxd::frac_bits < 0)
            return f;

        using Raw = Fxd::raw_type;
        constexpr Raw one = Fxd::one.raw_value;
        Raw result = 0;

        if (__builtin_sub_overflow(f.raw_value, one, &result))
            return f = handler(error::underflow);

        f.raw_value = result;

        if (f.raw_value != result)
            return f = handler(error::underflow);

        return f;
    }



    template<fixed_point Fxd,
             error_handler H>
    constexpr
    Fxd
    post_dec(Fxd& f,
             H&& handler)
        noexcept(IS_NOEXCEPT)
    {
        Fxd old = f;
        dec(f, std::forward<H>(handler));
        return old;
    }



    // Unary arithmetic


    template<fixed_point Fxd,
             error_handler H>
    constexpr
    Fxd
    negate(const Fxd& f,
           H&& handler)
        noexcept(IS_NOEXCEPT)
    {
        using Lim = std::numeric_limits<Fxd>;

        if constexpr (!Lim::is_signed)
            return f.raw_value == 0 ? f : handler(error::underflow);

        if (f == Lim::lowest())
            return handler(error::overflow);

        return -f;
    }



    // Binary arithmetic


    template<fixed_point Fxd,
             error_handler H>
    constexpr
    Fxd
    plus(const Fxd& a,
         const Fxd& b,
         H&& handler)
        noexcept(IS_NOEXCEPT)
    {
        using R = typename Fxd::raw_type;
        R r;
        if (__builtin_add_overflow(a.raw_value, b.raw_value, &r)) {
            if constexpr (std::numeric_limits<R>::is_signed)
                return handler(r < 0 ? error::overflow : error::underflow);
            else
                return handler(error::overflow);
        }

        return from_raw<Fxd>(r, std::forward<H>(handler));
    }


    template<fixed_point Fxd,
             error_handler H>
    constexpr
    Fxd
    minus(const Fxd& a,
          const Fxd& b,
          H&& handler)
        noexcept(IS_NOEXCEPT)
    {
        using R = typename Fxd::raw_type;
        R r;
        if (__builtin_sub_overflow(a.raw_value, b.raw_value, &r)) {
            if constexpr (std::numeric_limits<R>::is_signed)
                return handler(r < 0 ? error::overflow : error::underflow);
            else
                return handler(error::underflow);
        }

        return from_raw<Fxd>(r, std::forward<H>(handler));
    }


    template<fixed_point Fxd,
             error_handler H>
    Fxd
    multiplies(const Fxd& a,
               const Fxd& b,
               H&& handler)
        noexcept(IS_NOEXCEPT)
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
                    return handler(cc < 0 ? error::underflow : error::overflow);

            return from_raw<Fxd>(dd, std::forward<H>(handler));

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
                    return handler(c.second < 0 ? error::underflow : error::overflow);

            const R dd = static_cast<R>(d.first);

            if constexpr (std::numeric_limits<R>::is_signed) {
                // only valid values for d.second is 0 or -1
                if (d.second < -1)
                    return handler(error::underflow);
                if (d.second > 0)
                    return handler(error::overflow);
                // d.second is just a sign extension of the top bit of dd
                // if they disagree in sign, there's underflow or overflow
                if ((d.second < 0) != (dd < 0))
                    return handler(d.second < 0 ? error::underflow : error::overflow);

                return from_raw<Fxd>(dd, std::forward<H>(handler));
            } else {
                if (d.second)
                    return handler(error::overflow);
                return from_raw<Fxd>(d.first, std::forward<H>(handler));
            }

        }

    }


    template<fixed_point Fxd,
             error_handler H>
    Fxd
    divides(const Fxd& a,
            const Fxd& b,
            H&& handler)
        noexcept(IS_NOEXCEPT)
    {
        using R = typename Fxd::raw_type;

        if constexpr (has_wider_v<R>) {
            using W = wider_t<R>;
            const W aa = utils::shl<W>(a.raw_value, Fxd::frac_bits);
            const W bb = b.raw_value;

            if constexpr (std::numeric_limits<W>::is_signed) {
                if (aa == std::numeric_limits<W>::lowest()
                    && bb ==  -1)
                    return handler(error::overflow);
            }

            const W cc = aa / bb;
            return from_raw<Fxd>(cc, std::forward<H>(handler));
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

            auto q = utils::full_div(ua, ub, Fxd::frac_bits);
            auto r = utils::shrz(q, utils::type_width<U> - Fxd::frac_bits);

            if (neg_a != neg_b) {
                // must negate the result
                if (r.second // high bits are nonzero
                    || std::cmp_greater(r.first, std::numeric_limits<R>::max())) // too large
                    return handler(error::underflow);
                return from_raw<Fxd>(-static_cast<R>(r.first), std::forward<H>(handler));
            } else {
                if (r.second)
                    return handler(error::overflow);
                return from_raw<Fxd>(r.first, std::forward<H>(handler));
            }
        }
    }



#undef IS_NOEXCEPT



    namespace sat {


        template<fxd::fixed_point Fxd>
        [[nodiscard]]
        Fxd
        saturate(error e)
            noexcept
        {
            switch (e) {
                case error::underflow:
                    return std::numeric_limits<Fxd>::lowest();
                case error::overflow:
                    return std::numeric_limits<Fxd>::max();
                case error::div_by_zero:
                    std::raise(SIGFPE);
                    return 0;
                default:
                    std::abort();
            }
        }


        template<fxd::fixed_point Fxd,
                 std::integral I>
        constexpr
        Fxd
        make_fixed(const I& src)
            noexcept
        {
            return make_fixed<Fxd>(src, saturate<Fxd>);
        }


        template<fxd::fixed_point Fxd,
                 std::floating_point Flt>
        constexpr
        Fxd
        make_fixed(const Flt& src)
            noexcept
        {
            return make_fixed<Fxd>(src, saturate<Fxd>);
        }


        template<int Int,
                 int Frac,
                 typename T = fxd::select_int_t<Int + Frac>,
                 std::convertible_to<fxd::fixed<Int, Frac, T>> Src>
        constexpr
        fxd::fixed<Int, Frac, T>
        make_fixed(const Src& src)
            noexcept
        {
            using Fxd = fxd::fixed<Int, Frac, T>;
            return make_fixed<Fxd>(src, saturate<Fxd>);
        }


        template<int Int,
                 int Frac,
                 typename T = fxd::select_uint_t<Int + Frac>,
                 std::convertible_to<fxd::fixed<Int, Frac, T>> Src>
        constexpr
        fxd::fixed<Int, Frac, T>
        make_ufixed(const Src& src)
            noexcept
        {
            using Fxd = fxd::fixed<Int, Frac, T>;
            return make_fixed<Fxd>(src, saturate<Fxd>);
        }


        template<fixed_point Fxd,
                 std::convertible_to<Fxd> Src>
        constexpr
        Fxd&
        assign(Fxd& dst,
               Src src)
            noexcept
        {
            return assign(dst, src, saturate<Fxd>);
        }


        template<fixed_point Fxd>
        constexpr
        Fxd&
        inc(Fxd& f)
            noexcept
        {
            return inc(f, saturate<Fxd>);
        }



        template<fixed_point Fxd>
        constexpr
        Fxd
        post_inc(Fxd& f)
            noexcept
        {
            return post_inc(f, saturate<Fxd>);
        }


        template<fixed_point Fxd>
        constexpr
        Fxd&
        dec(Fxd& f)
            noexcept
        {
            return dec(f, saturate<Fxd>);
        }



        template<fixed_point Fxd>
        constexpr
        Fxd
        post_dec(Fxd& f)
            noexcept
        {
            return post_dec(f, saturate<Fxd>);
        }


        template<fixed_point Fxd>
        constexpr
        Fxd
        negate(const Fxd& f)
            noexcept
        {
            return negate(f, saturate<Fxd>);
        }


        template<fixed_point Fxd>
        constexpr
        Fxd
        plus(const Fxd& a,
             const Fxd& b)
            noexcept
        {
            return plus(a, b, saturate<Fxd>);
        }



        template<fixed_point Fxd>
        constexpr
        Fxd
        minus(const Fxd& a,
              const Fxd& b)
            noexcept
        {
            return minus(a, b, saturate<Fxd>);
        }


        template<fixed_point Fxd>
        constexpr
        Fxd
        multiplies(const Fxd& a,
                   const Fxd& b)
            noexcept
        {
            return multiplies(a, b, saturate<Fxd>);
        }


        template<fixed_point Fxd>
        constexpr
        Fxd
        divides(const Fxd& a,
                const Fxd& b)
            noexcept
        {
            return divides(a, b, saturate<Fxd>);
        }



    } // namespace sat




}


#endif
