#include "gtest/gtest.h"
#include "../../antity/src/core/ECS/Component.h"

TEST(Components,TypeId)
{
	ASSERT_EQ(ant::Component<int>::GetTypeID(), ant::Component<int>::GetTypeID());
	ASSERT_NE(ant::Component<int>::GetTypeID(), ant::Component<float>::GetTypeID());
}
