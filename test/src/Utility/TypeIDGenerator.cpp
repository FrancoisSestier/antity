#include <gtest/gtest.h>
#include <antity/core/Identifier.h>
using namespace ant;

TEST(TypeIDGen, genID)
{
	ASSERT_EQ(TypeIdGenerator::GetTypeID<int>(), TypeIdGenerator::GetTypeID<int>());
	ASSERT_EQ(TypeIdGenerator::GetTypeID<int>(), TypeIdGenerator::GetTypeID<int>());
	ASSERT_NE(TypeIdGenerator::GetTypeID<int>(), TypeIdGenerator::GetTypeID<float>());
	ASSERT_EQ(TypeIdGenerator::GetTypeID<float>(), TypeIdGenerator::GetTypeID<float>());
	ASSERT_EQ(TypeIdGenerator::GetTypeID<float>(), TypeIdGenerator::GetTypeID<float>());

}
