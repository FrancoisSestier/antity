#pragma once
#include "TypeTraits.h"

template <typename... Ts>
struct vector_tuple {
    using type = std::tuple<std::vector<Ts>...>;
};

template <typename... Ts>
struct is_vector_tuple : std::false_type {};

template <typename... Ts>
struct is_vector_tuple<std::tuple<std::vector<Ts>...>> : std::true_type {};

template <typename T>
concept Vector_Tuple = is_vector_tuple<T>::value;

template <typename T, Vector_Tuple VectorTuple>
std::vector<T>& get(VectorTuple vectorTuple) {
    return std::get<std::vector<T>, vectorTuple>;
};
