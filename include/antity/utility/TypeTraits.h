#pragma once
#include <type_traits>
#include <vector>

namespace ant {

    template <typename>
    struct is_vector : std::false_type {};

    template <typename T, typename A>
    struct is_vector<std::vector<T, A>> : std::true_type {};

    template <typename T>
    concept Vector = is_vector<T>::value;

}  // namespace ant