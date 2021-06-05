#pragma once
#include <tuple>
#include <type_traits>

namespace ant {

    template <class... T>
    struct type_list {
        using type = std::tuple<T...>;
    };

    template <class T, class Tuple>
    struct type_index;

    template <class T, class... Types>
    struct type_index<T, std::tuple<T, Types...>> {
        static const std::size_t value = 0;
    };

    template <class T, class U, class... Types>
    struct type_index<T, std::tuple<U, Types...>> {
        static const std::size_t value
            = 1 + type_index<T, std::tuple<Types...>>::value;
    };

    template <typename T, typename Tuple>
    struct contains_type;

    template <typename T, typename... Us>
    struct contains_type<T, std::tuple<Us...>>
        : std::disjunction<std::is_same<T, Us>...> {};

    template <class T, typename... Ts>
    struct add_type {
        using type = typename std::tuple<T, Ts...>;
    };

    template <size_t N, typename tuple>
    struct extract : std::tuple_element<N, tuple> {};

    template <typename... Ts>
    struct is_tuple : std::false_type {};

    template <typename... Ts>
    struct is_tuple<std::tuple<Ts...>> : std::true_type {};

    template <typename T>
    concept Tuple = is_tuple<T>::value;
}  // namespace ant
