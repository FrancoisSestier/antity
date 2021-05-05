#include <gtest/gtest.h>
#include "../../antity/src/core/ECS/Identifier.h"

using namespace ant;

TEST(TypeIDGen, genID)
{
	ASSERT_EQ(TypeIdGenerator<int>::GetNewID<int>(), TypeIdGenerator<int>::GetNewID<int>());
	ASSERT_NE(TypeIdGenerator<int>::GetNewID<int>(), TypeIdGenerator<int>::GetNewID<float>());
	ASSERT_EQ(TypeIdGenerator<float>::GetNewID<int>(), TypeIdGenerator<float>::GetNewID<int>());
}
