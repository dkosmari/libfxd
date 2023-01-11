#ifndef LIBFXD_UTILS_TUPLE_HPP
#define LIBFXD_UTILS_TUPLE_HPP


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


    template<tuple_like Tup>
    constexpr
    last_element_t<Tup>
    last(const Tup& t)
        noexcept
    {
        return get<std::tuple_size_v<Tup> - 1>(t);
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


}


#endif
