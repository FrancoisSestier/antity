#include <gtest/gtest.h>

#include <antity/core/identifier.hpp>
#include <antity/utility/function_traits.hpp>

using namespace ant;

TEST(TypeIDGen, genID) {
    ASSERT_EQ(type_id_generator::get<int>(), type_id_generator::get<int>());
    ASSERT_EQ(type_id_generator::get<int>(), type_id_generator::get<int>());
    ASSERT_NE(type_id_generator::get<int>(), type_id_generator::get<float>());
    ASSERT_EQ(type_id_generator::get<float>(), type_id_generator::get<float>());
    ASSERT_EQ(type_id_generator::get<float>(), type_id_generator::get<float>());

    ASSERT_EQ(get_type_signature<int>(), get_type_signature<int>());
    ASSERT_EQ(get_type_signature<float>(), get_type_signature<float>());
    ASSERT_EQ(get_type_signature<int>(), std::bitset<_max_components>(1ULL));
    ASSERT_EQ(
        get_type_signature<float>(),
        std::bitset<_max_components>(1ULL << type_id_generator::get<float>()));
}

TEST(function_traits, all) {
    int a = 2;
    int b = 3;
    auto lambda = [&](float t) -> float {
        b = static_cast<int>(t);
        return t + b;
    };

    using lambda_traits = functor_traits<decltype(lambda)>;

    static_assert(std::is_same_v<lambda_traits::args_type, type_list<float>>,
                  "");
    static_assert(std::is_same_v<lambda_traits::return_type, float>,
                  "");
}