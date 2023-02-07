/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_INCLUDING_IMPL_SAFE_HPP
#error "Do not include this header directly. Either include 'saturate.hpp' or 'except.hpp'."
#endif

#ifndef LIBFXD_HERE
#error "Current namespace name not defined."
#endif


// Constructors

template<fixed_point Fxd,
         std::integral I>
constexpr
Fxd
from_raw(I val)
{
    Fxd result = Fxd::from_raw(val);

    if constexpr (impl::type_width<I> > Fxd::bits) {
        // check if significant bits got chopped off
        if (std::cmp_not_equal(result.raw_value, val))
            return handler<Fxd>(val < 0
                                ? impl::error::underflow
                                : impl::error::overflow);
    }
    return result;
}


template<fixed_point Fxd>
Fxd
make_fixed(Fxd src)
    noexcept
{
    return src;
}



template<fixed_point Fxd,
         std::integral I>
constexpr
Fxd
make_fixed(I val)
{

    if constexpr (Fxd::frac_bits < 0) {

        // Shifting right.
        if (val < 0)
            val += impl::make_bias_for(-Fxd::frac_bits, val);

        auto raw = impl::shr_real(val, -Fxd::frac_bits);
        return LIBFXD_HERE::from_raw<Fxd>(raw);

    } else {

        // Shifting left.
        // Use max bits available to minimize spurious overflows.
        using IWide = impl::max_int_for<I>;
        auto [raw, overflow] = impl::overflow::shl_real<IWide>(val,
                                                               Fxd::frac_bits);

        if (overflow)
            return handler<Fxd>(val < 0
                                ? impl::error::underflow
                                : impl::error::overflow);

        return LIBFXD_HERE::from_raw<Fxd>(raw);

    }

}


template<fixed_point Fxd,
         std::floating_point Flt>
Fxd
make_fixed(Flt val)
{
    using Lim = std::numeric_limits<Fxd>;

    if (!std::isfinite(val)) {
        if (std::isnan(val))
            return handler<Fxd>(impl::error::not_a_number);
        return handler<Fxd>(val < 0
                            ? impl::error::underflow
                            : impl::error::overflow);
    }

    if (val < static_cast<Flt>(Lim::lowest()))
        return handler<Fxd>(impl::error::underflow);

    if (val > static_cast<Flt>(Lim::max()))
        return handler<Fxd>(impl::error::overflow);

    return Fxd{val};
}


template<fxd::unsigned_fixed_point Fxd,
         std::convertible_to<Fxd> Src>
constexpr
Fxd
make_ufixed(Src src)
{
    return LIBFXD_HERE::make_fixed<Fxd>(src);
}


template<int Int,
         int Frac,
         typename I = impl::select_int_t<Int + Frac>,
         std::convertible_to<fxd::fixed<Int, Frac, I>> Src>
constexpr
fxd::fixed<Int, Frac, I>
make_fixed(Src src)
{
    using Fxd = fxd::fixed<Int, Frac, I>;
    return LIBFXD_HERE::make_fixed<Fxd>(src);
}


template<int Int,
         int Frac,
         typename U = impl::select_uint_t<Int + Frac>,
         std::convertible_to<fxd::fixed<Int, Frac, U>> Src>
constexpr
fxd::fixed<Int, Frac, U>
make_ufixed(Src src)
{
    using Fxd = fxd::fixed<Int, Frac, U>;
    return LIBFXD_HERE::make_fixed<Fxd>(src);
}



// Casting (fixed -> fixed)


template<fixed_point Dst,
         fixed_point Src>
constexpr
Dst
fixed_cast(Src src)
{
    using SrcRaw = typename Src::raw_type;
    SrcRaw sraw = src.raw_value;

    if constexpr (!std::numeric_limits<Dst>::is_signed
                  &&
                  std::numeric_limits<Src>::is_signed)
        if (sraw < 0)
            return handler<Dst>(impl::error::underflow);

    constexpr int diff = Dst::frac_bits - Src::frac_bits;
    if constexpr (diff < 0) {

        // shifting right
        if (sraw < 0)
            sraw += impl::make_bias_for(-diff, sraw);

        auto draw = impl::shr_real(sraw, -diff);

        return LIBFXD_HERE::from_raw<Dst>(draw);

    } else {

        // shifting left
        // use more bits to avoid spurious overflows
        using SrcWide = impl::max_int_for<SrcRaw>;

        auto [draw, overflow] = impl::overflow::shl_real<SrcWide>(sraw, diff);
        if (overflow)
            return handler<Dst>(sraw < 0
                                ? impl::error::underflow
                                : impl::error::overflow);
        return LIBFXD_HERE::from_raw<Dst>(draw);

    }

}


template<unsigned_fixed_point Dst,
         fixed_point Src>
constexpr
Dst
ufixed_cast(Src src)
{
    return LIBFXD_HERE::fixed_cast<Dst>(src);
}


template<int Int,
         int Frac,
         typename Raw = impl::select_int_t<Int + Frac>,
         fixed_point Src>
constexpr
fixed<Int, Frac, Raw>
fixed_cast(Src src)
{
    return LIBFXD_HERE::fixed_cast<fixed<Int, Frac, Raw>>(src);
}


template<int Int,
         int Frac,
         typename Raw = impl::select_uint_t<Int + Frac>,
         fixed_point Src>
constexpr
fixed<Int, Frac, Raw>
ufixed_cast(Src src)
{
    return LIBFXD_HERE::fixed_cast<fixed<Int, Frac, Raw>>(src);
}



// Conversions (fixed -> primitive)


template<std::integral I,
         fixed_point Fxd>
constexpr
I
to_int(Fxd f)
{
    using Raw = typename Fxd::raw_type;
    Raw raw = f.raw_value;

    using LimI = std::numeric_limits<I>;

    if constexpr (!LimI::is_signed && std::numeric_limits<Raw>::is_signed) {
        if (raw < 0)
            return handler<I>(impl::error::underflow);
    }

    if constexpr (Fxd::frac_bits >= 0) {

        if (raw < 0) // if negative, add a bias before shifting
            raw += impl::make_bias_for(Fxd::frac_bits, raw);

        raw = impl::shr_real(raw, Fxd::frac_bits);

        if (std::cmp_less(raw, LimI::min()))
            return handler<I>(impl::error::underflow);

        if (std::cmp_greater(raw, LimI::max()))
            return handler<I>(impl::error::overflow);

        return raw;

    } else {

        // Allow left-shifting to happen on a wider type.
        using RR = impl::max_int_for<Raw>;
        auto [result, ovf] = impl::overflow::shl_real<RR>(raw, -Fxd::frac_bits);
        if (ovf)
            return handler<I>(raw < 0
                                ? impl::error::underflow
                                : impl::error::overflow);

        if (std::cmp_less(result, LimI::min()))
            return handler<I>(impl::error::underflow);

        if (std::cmp_greater(result, LimI::max()))
            return handler<I>(impl::error::overflow);

        return result;

    }

}


template<fixed_point Fxd>
requires impl::has_int_for<Fxd::int_bits, typename Fxd::raw_type>
constexpr
impl::select_int_for<Fxd::int_bits, typename Fxd::raw_type>
to_int(Fxd f)
{
    using I = impl::select_int_for<Fxd::int_bits, typename Fxd::raw_type>;
    return LIBFXD_HERE::to_int<I>(f);
}



// Operators


template<fixed_point Fxd,
         std::convertible_to<Fxd> Src>
constexpr
Fxd&
assign(Fxd& dst,
       Src src)
{
    return dst = LIBFXD_HERE::make_fixed<Fxd>(src);
}



template<fixed_point Fxd>
constexpr
Fxd&
inc(Fxd& f)
{
    // if no integral bit, it always overflows
    if constexpr (Fxd::int_bits < 1)
        return f = handler<Fxd>(impl::error::overflow);

    auto [result, carry] = impl::overflow::add(f.raw_value, Fxd{1}.raw_value);
    if (carry)
        return f = handler<Fxd>(impl::error::overflow);

    return f = LIBFXD_HERE::from_raw<Fxd>(result);
}


template<fixed_point Fxd>
constexpr
Fxd
post_inc(Fxd& f)
{
    Fxd old = f;
    LIBFXD_HERE::inc(f);
    return old;
}


template<fixed_point Fxd>
constexpr
Fxd&
dec(Fxd& f)
{
    // if no integral bit, it always underflows
    if constexpr (Fxd::int_bits < 1)
        return f = handler<Fxd>(impl::error::underflow);

    auto [result, overflow] = impl::overflow::sub(f.raw_value, Fxd{1}.raw_value);
    if (overflow)
        return f = handler<Fxd>(impl::error::underflow);

    return f = LIBFXD_HERE::from_raw<Fxd>(result);
}


template<fixed_point Fxd>
constexpr
Fxd
post_dec(Fxd& f)
{
    Fxd old = f;
    LIBFXD_HERE::dec(f);
    return old;
}


template<fixed_point Fxd>
constexpr
Fxd
negate(Fxd f)
{
    using Lim = std::numeric_limits<Fxd>;

    if constexpr (!Lim::is_signed)
        return f.raw_value == 0
            ? f
            : handler<Fxd>(impl::error::underflow);

    if (f == Lim::lowest())
        return handler<Fxd>(impl::error::overflow);

    return -f;
}


template<fixed_point Fxd>
constexpr
Fxd
add(Fxd a,
    Fxd b)
{
    auto [result, overflow] = impl::overflow::add(a.raw_value, b.raw_value);

    if (overflow) {
        if constexpr (std::numeric_limits<Fxd>::is_signed)
            return handler<Fxd>(a.raw_value < 0
                                ? impl::error::underflow
                                : impl::error::overflow);
        else
            return handler<Fxd>(impl::error::overflow);
    }

    return LIBFXD_HERE::from_raw<Fxd>(result);
}


template<fixed_point Fxd>
constexpr
Fxd
sub(Fxd a,
    Fxd b)
{
    auto [result, overflow] = impl::overflow::sub(a.raw_value, b.raw_value);

    if (overflow) {
        if constexpr (std::numeric_limits<Fxd>::is_signed)
            return handler<Fxd>(a.raw_value < 0
                                ? impl::error::underflow
                                : impl::error::overflow);
        else
            return handler<Fxd>(impl::error::underflow);
    }

    return LIBFXD_HERE::from_raw<Fxd>(result);
}


#include "safe-mul.hpp"


#include "safe-div.hpp"


using namespace zero;


template<fixed_point Fxd>
constexpr
Fxd
abs(Fxd f)
{
    if (f.raw_value >= 0)
        return f;
    return LIBFXD_HERE::negate(f);
}



template<fixed_point Fxd>
constexpr
Fxd
fdim(Fxd a,
     Fxd b)
{
    if (a > b)
        return LIBFXD_HERE::sub(a, b);
    else
        return 0;
}


template<fixed_point Fxd>
constexpr
Fxd
nextafter(Fxd from,
          Fxd to)
{
    constexpr Fxd e = std::numeric_limits<Fxd>::epsilon();
    if (from < to)
        return LIBFXD_HERE::add(from, e);
    if (from > to)
        return LIBFXD_HERE::sub(from, e);
    return to;
}
