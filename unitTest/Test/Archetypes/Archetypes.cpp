#include <gtest/gtest.h>
#include "../../antity/src/core/ECS/Archetype.h"

using namespace ant;
TEST(Archetypes, Creation) {

	type_list<int, char, float> typeList;

	auto archetype1 = Archetype<decltype(typeList)>(0);
	auto archetype2 = Archetype<int,char,float>(1);
	auto archetype3 = Archetype <type_list<int, char, float>>(0) ;

	ASSERT_TRUE(archetype1.contains<int>());
	ASSERT_TRUE(archetype1.contains<char>());
	ASSERT_TRUE(archetype1.contains<float>());
	
	ASSERT_TRUE(archetype2.contains<int>());
	ASSERT_TRUE(archetype2.contains<char>());
	ASSERT_TRUE(archetype2.contains<float>());

	ASSERT_TRUE(archetype3.contains<int>());
	ASSERT_TRUE(archetype3.contains<char>());
	ASSERT_TRUE(archetype3.contains<float>());
	
	ASSERT_TRUE(archetype1.size() == 3);
	ASSERT_TRUE(archetype2.size() == 3);
	ASSERT_TRUE(archetype3.size() == 3);

	auto comp = ArchetypeBase::comparator();
	
	ASSERT_TRUE(!comp(&archetype1,&archetype2));
	ASSERT_TRUE(!comp(&archetype2, &archetype3));
	ASSERT_TRUE(comp(&archetype1, &archetype3));
}

