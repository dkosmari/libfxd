#ifndef LIBFXD_SAFE_OPERATORS_HPP
#define LIBFXD_SAFE_OPERATORS_HPP

#include <concepts>
#include <csignal>
#include <cstdlib>
#include <stdexcept>
#include <utility> // in_range(), forward()

#include "fixed.hpp"
#include "concepts.hpp"
#include "limits.hpp"
#include "traits.hpp"


namespace fxd::safe {


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
    raw_clamped(T x,
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
        typename Fxd::raw_type r;
        if (__builtin_add_overflow(a.raw_value, b.raw_value, &r))
            return handler(r < 0 ? error::overflow : error::underflow);

        return raw_clamped<Fxd>(r, std::forward<H>(handler));
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
        typename Fxd::raw_type r;
        if (__builtin_sub_overflow(a.raw_value, b.raw_value, &r))
            return handler(r < 0 ? error::overflow : error::underflow);

        return raw_clamped<Fxd>(r, std::forward<H>(handler));
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
            const W bb = Fxd{b}.raw_value;

            const W cc = aa * bb;
            const W dd = utils::shr<W>(cc, Fxd::frac_bits);

            if constexpr (Fxd::frac_bits < 0)
                // dd could have lost significant bits
                if (cc != utils::shl<W>(dd, Fxd::frac_bits))
                    return handler(cc < 0 ? error::underflow : error::overflow);

            return raw_clamped<Fxd>(dd, std::forward<H>(handler));

        } else {

            const auto c = utils::full_mult(a.raw_value, b.raw_value);
            const auto d = utils::shr(c, Fxd::frac_bits);

            if constexpr (Fxd::frac_bits < 0) {
                // will actually shift left
                // check if shifting up lost significant bits
                if (c != utils::shl(d, Fxd::frac_bits))
                    return handler(c.second < 0 ? error::underflow : error::overflow);
            }

            const R dd = static_cast<R>(d.first);

            if (d.second < 0) {
                if (d.second != -1 || dd >= 0)
                    return handler(error::underflow);
            } else {
                if (d.second || dd < 0)
                    return handler(error::overflow);
            }
            return raw_clamped<Fxd>(dd, std::forward<H>(handler));

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

            if (std::numeric_limits<W>::is_signed
                && aa == std::numeric_limits<W>::lowest()
                && bb ==  -1)
                return handler(error::overflow);

            const W cc = aa / bb;
            return raw_clamped<Fxd>(cc, std::forward<H>(handler));
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

            auto q = utils::full_div<U>(ua, ub, Fxd::frac_bits);

            // TODO: this can overflow
            // a.raw_value = utils::shl(q, Fxd::frac_bits).first;

            // TODO: this can overflow
            // if (a_neg != b_neg)
            //     a.raw_value = -a.raw_value;
        }
        return a;
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


    } // namespace sat




}


#endif
