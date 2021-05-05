#include <gtest/gtest.h>
#include "../../antity/src/core/ECS/ArchetypeMap.h"

using namespace ant;

TEST(ArchetypeMap, Getter) {

	type_list<int, char, float> typeList;

	ArchetypeMap archetypeMap;
	
	auto archetype1 = archetypeMap.GetArchetype<type_list<int, char, float>>();
	auto archetype2 = archetypeMap.GetArchetype<char,int, float>();
	auto archetype3 = archetypeMap.GetArchetype<char, float>();
	auto archetype4 = archetypeMap.GetArchetype<char, float>(2);

	auto comp = ArchetypeBase::comparator();

	ASSERT_TRUE(comp(archetype1, archetype2));
	ASSERT_TRUE(!comp(archetype3, archetype2));
	ASSERT_TRUE(!comp(archetype4, archetype3));


}
