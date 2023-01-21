#ifndef LIBFXD_INCLUDING_UTILS_SAFE_HPP_IMPL
#error "Do not include this header directly. Either include 'saturate.hpp' or 'except.hpp'."
#endif


template<fxd::fixed_point Fxd,
         std::integral I>
constexpr
Fxd
from_raw(I val)
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
requires (Fxd::frac_bits < 0)
constexpr
Fxd
make_fixed(I val)
{
    if (val < 0)
        val += utils::shift::make_bias_for(-Fxd::frac_bits, val);

    using Raw = typename Fxd::raw_type;
    const Raw shifted_val = utils::shift::shr_real<Raw>(val, -Fxd::frac_bits);
    return from_raw<Fxd>(shifted_val);
}



template<fxd::fixed_point Fxd,
         std::integral I>
requires (Fxd::frac_bits >= 0)
constexpr
Fxd
make_fixed(I val)
{
    using Raw = typename Fxd::raw_type;
    using Lim = std::numeric_limits<Raw>;
    constexpr int w = type_width<Raw>;

    // ensure val can be represented as raw_type
    if (std::cmp_less(val, Lim::min()))
        return handler<Fxd>(error::underflow);
    if (std::cmp_greater(val, Lim::max()))
        return handler<Fxd>(error::overflow);

    // top are the bits that will be thrown out after the shift
    const Raw top = utils::shift::shr_real<Raw>(val, w - Fxd::frac_bits);
    const Raw shifted_val = utils::shift::shl_real<Raw>(val, Fxd::frac_bits);

    // ensure top is only either 0 or -1
    if (top > 0)
        return handler<Fxd>(error::overflow);
    if (std::cmp_less(top, -1))
        return handler<Fxd>(error::underflow);

    // ensure shifted_val and top have the same sign
    if ((shifted_val < 0) != (top < 0))
        return handler<Fxd>(top < 0
                            ? error::underflow
                            : error::overflow);

    return from_raw<Fxd>(shifted_val);
}



template<fxd::fixed_point Fxd,
         std::floating_point Flt>
Fxd
make_fixed(Flt val)
{
    using Lim = std::numeric_limits<Fxd>;

    if (!std::isfinite(val)) {
        if (std::isnan(val))
            return handler<Fxd>(error::not_a_number);
        return handler<Fxd>(val < 0
                            ? error::underflow
                            : error::overflow);
    }

    if (val < static_cast<Flt>(Lim::lowest()))
        return handler<Fxd>(error::underflow);

    if (val > static_cast<Flt>(Lim::max()))
        return handler<Fxd>(error::overflow);

    return val;
}



template<int Int,
         int Frac,
         typename I = fxd::select_int_t<Int + Frac>,
         std::convertible_to<fxd::fixed<Int, Frac, I>> Src>
constexpr
fxd::fixed<Int, Frac, I>
make_fixed(Src src)
{
    using Fxd = fxd::fixed<Int, Frac, I>;
    return make_fixed<Fxd>(src);
}



template<int Int,
         int Frac,
         typename U = fxd::select_uint_t<Int + Frac>,
         std::convertible_to<fxd::fixed<Int, Frac, U>> Src>
constexpr
fxd::fixed<Int, Frac, U>
make_ufixed(Src src)
{
    using Fxd = fxd::fixed<Int, Frac, U>;
    return make_fixed<Fxd>(src);
}



template<fixed_point Fxd,
         std::convertible_to<Fxd> Src>
constexpr
Fxd&
assign(Fxd& dst,
       Src src)
{
    return dst = make_fixed<Fxd>(src);
}



template<fixed_point Fxd>
constexpr
Fxd&
inc(Fxd& f)
{
    // if no integral bit, it always overflows
    if constexpr (Fxd::int_bits < 1)
        return f = handler<Fxd>(error::overflow);

    auto [result, carry] = utils::overflow::add(f.raw_value, Fxd{1}.raw_value);
    if (carry)
        return f = handler<Fxd>(error::overflow);

    f.raw_value = result;

    if (f.raw_value != result)
        return f = handler<Fxd>(error::overflow);

    return f;
}


template<fixed_point Fxd>
constexpr
Fxd
post_inc(Fxd& f)
{
    Fxd old = f;
    inc(f);
    return old;
}


template<fixed_point Fxd>
constexpr
Fxd&
dec(Fxd& f)
{
    // if no integral bit, it always underflows
    if constexpr (Fxd::int_bits < 1)
        return f = handler<Fxd>(error::underflow);

    auto [result, overflow] = utils::overflow::sub(f.raw_value, Fxd{1}.raw_value);
    if (overflow)
        return f = handler<Fxd>(error::underflow);

    f.raw_value = result;

    if (f.raw_value != result)
        return f = handler<Fxd>(error::underflow);

    return f;
}


template<fixed_point Fxd>
constexpr
Fxd
post_dec(Fxd& f)
{
    Fxd old = f;
    dec(f);
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
            : handler<Fxd>(error::underflow);

    if (f == Lim::lowest())
        return handler<Fxd>(error::overflow);

    return -f;
}


template<fixed_point Fxd>
constexpr
Fxd
add(Fxd a,
    Fxd b)
{
    auto [result, overflow] = utils::overflow::add(a.raw_value, b.raw_value);

    if (overflow) {
        if constexpr (std::numeric_limits<Fxd>::is_signed)
            return handler<Fxd>(a.raw_value < 0
                                ? error::underflow
                                : error::overflow);
        else
            return handler<Fxd>(error::overflow);
    }

    return from_raw<Fxd>(result);
}


template<fixed_point Fxd>
constexpr
Fxd
sub(Fxd a,
    Fxd b)
{
    auto [result, overflow] = utils::overflow::sub(a.raw_value, b.raw_value);

    if (overflow) {
        if constexpr (std::numeric_limits<Fxd>::is_signed)
            return handler<Fxd>(a.raw_value < 0
                                ? error::underflow
                                : error::overflow);
        else
            return handler<Fxd>(error::underflow);
    }

    return from_raw<Fxd>(result);
}


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
    using utils::shift::shr;
    using utils::shift::shl;

    constexpr int w = type_width<typename Fxd::raw_type>;
    // offset used for shifting left.
    constexpr int offset = w - Fxd::frac_bits;

    auto c = utils::mul::mul<Fxd::bits>(a.raw_value, b.raw_value);

    if (is_negative(c)) {
        // negative numbers need a bias to zero when shifting
        const auto bias = utils::shift::make_bias_for(Fxd::frac_bits, c);
        c = utils::add::add(c, bias);
    }

    const auto d = shl(c, offset);
    // unshift, check that the value matches
    if (offset > 0 && c != shr(d, offset))
        return handler<Fxd>(is_negative(c)
                            ? error::underflow
                            : error::overflow);

    return from_raw<Fxd>(utils::tuple::last(d));
}



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

    const auto d = utils::shift::shl(c, offset);

    // unshift to ensure high significant bits were not lost
    if (offset > 0 && c != utils::shift::shr(d, offset))
        return handler<Fxd>(c < 0
                            ? error::underflow
                            : error::overflow);

    return from_raw<Fxd>(d);
}



template<fixed_point Fxd>
constexpr
Fxd
abs(Fxd f)
{
    if (f.raw_value >= 0)
        return f;
    return negate(f);
}



template<fixed_point Fxd>
constexpr
Fxd
fdim(Fxd a,
     Fxd b)
{
    if (a > b)
        return sub(a, b);
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
        return add(from, e);
    if (from > to)
        return sub(from, e);
    return to;
}
