#include <gtest/gtest.h>
#include <antity/core/identifier.hpp>
using namespace ant;

TEST(TypeIDGen, genID)
{
	ASSERT_EQ(type_id_generator::get<int>(), type_id_generator::get<int>());
	ASSERT_EQ(type_id_generator::get<int>(), type_id_generator::get<int>());
	ASSERT_NE(type_id_generator::get<int>(), type_id_generator::get<float>());
	ASSERT_EQ(type_id_generator::get<float>(), type_id_generator::get<float>());
	ASSERT_EQ(type_id_generator::get<float>(), type_id_generator::get<float>());

	ASSERT_EQ(get_type_signature<int>(), get_type_signature<int>());
	ASSERT_EQ(get_type_signature<float>(), get_type_signature<float>());
	ASSERT_EQ(get_type_signature<int>(), std::bitset<_max_components>(1ULL));
	ASSERT_EQ(get_type_signature<float>(), std::bitset<_max_components>(1ULL << type_id_generator::get<float>()));


}
