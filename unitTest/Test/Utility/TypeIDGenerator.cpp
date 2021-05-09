#include <gtest/gtest.h>
#include "../../antity/src/core/ECS/Identifier.hpp"
using namespace ant;

TEST(TypeIDGen, genID)
{
	ASSERT_EQ(TypeIdGenerator::GetTypeID<int>(), TypeIdGenerator::GetTypeID<int>());
	ASSERT_NE(TypeIdGenerator::GetTypeID<int>(), TypeIdGenerator::GetTypeID<float>());

}
