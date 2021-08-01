#include <gtest/gtest.h>
#include <antity/core/registry.hpp>
using namespace ant;

TEST(Registry,Createentity)
{
	registry reg;
	entity_t entityA = reg.create();
	entity_t entityB = reg.create();

	ASSERT_NE(entityA, entityB);
	ASSERT_EQ(entityA, 1);
	
}

TEST(Registry, RegisterComponent)
{
	registry reg;
	reg.save<int>();
	reg.save<char>();
	
}

TEST(Registry, AddComponent)
{
	registry reg;
	
	reg.save<int>();
	reg.save<float>();

	entity_t entity_t = reg.create();
	
	reg.add<int>(entity_t, 1);
	reg.add<float>(entity_t, .4f);
	
}

TEST(Registry, get_components)
{
	registry reg;

	chunk_id_t id = 0;

	reg.save<int>();
	reg.save<float>();
	reg.save<char>();

	entity_t entity = reg.create();
	entity_t other = reg.create();

	reg.add<int>(entity, 1);
	reg.add<int>(other, 10);
	reg.add<float>(entity, .4f);
	reg.add<char>(other, 'c');

	int counter = 0;

	reg.get<int>();

	for(auto[e,i,f] : reg.get<int, float>())
	{
		
		counter ++;
		ASSERT_EQ(e, entity);
		ASSERT_EQ(i, 1);
		ASSERT_EQ(f, .4f);
		f = .5f;
	}

	auto [j] = reg.get_entity_components<float>(entity);
	ASSERT_EQ(j, .5f);

	ASSERT_EQ(counter, 1);
	
}
