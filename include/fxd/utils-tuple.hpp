#ifndef LIBFXD_UTILS_TUPLE_HPP
#define LIBFXD_UTILS_TUPLE_HPP

#include <concepts>
#include <tuple>
#include <utility>


namespace fxd::utils::tuple {


    template<typename T>
    concept tuple_like = std::integral<typename std::tuple_size<T>::value_type>;


    template<tuple_like Tup>
    struct first_element {
        using type = std::tuple_element_t<0, Tup>;
    };

    template<tuple_like Tup>
    using first_element_t = typename first_element<Tup>::type;



    template<tuple_like Tup>
    struct last_element {
        using type = std::tuple_element_t<std::tuple_size_v<Tup> - 1, Tup>;
    };

    template<tuple_like Tup>
    using last_element_t = typename last_element<Tup>::type;




    template<tuple_like Tup>
    constexpr
    first_element_t<Tup>
    first(const Tup& t)
        noexcept
    {
        return get<0>(t);
    }


    // treat it as a tuple, but unsigned
    template<std::integral I>
    constexpr
    std::make_unsigned_t<half_width_t<I>>
    first(I i)
        noexcept
    {
        using U = std::make_unsigned_t<half_width_t<I>>;
        return static_cast<U>(i);
    }


    template<tuple_like Tup>
    constexpr
    last_element_t<Tup>
    last(const Tup& t)
        noexcept
    {
        return get<std::tuple_size_v<Tup> - 1>(t);
    }


    // treat it as a tuple
    template<std::integral I>
    constexpr
    half_width_t<I>
    last(I i)
        noexcept
    {
        using N = half_width_t<I>;
        constexpr int h = type_width<I> / 2;
        return static_cast<N>(i >> h);
    }


    template<template<typename...> typename Tuple,
             typename Head,
             typename... Tail,
             std::size_t... Idx>
    requires tuple_like<Tuple<Head, Tail...>>
    constexpr
    std::tuple<Tail...>
    tail_helper(const Tuple<Head, Tail...>& t,
                std::index_sequence<Idx...>)
        noexcept
    {
        return { get<1 + Idx>(t) ... };
    }


    template<template<typename...> typename Tuple,
             typename Head,
             typename... Tail>
    requires tuple_like<Tuple<Head, Tail...>>
    constexpr
    std::tuple<Tail...>
    tail(const Tuple<Head, Tail...>& t)
        noexcept
    {
        using Idx = std::index_sequence_for<Tail...>;
        return tail_helper(t, Idx{});
    }



    template<tuple_like Tup>
    constexpr
    bool
    is_negative(const Tup& t)
        noexcept
    {
        return last(t) < 0;
    }


    template<std::integral I>
    constexpr
    bool
    is_negative(I i)
        noexcept
    {
        return i < 0;
    }


}


#endif
