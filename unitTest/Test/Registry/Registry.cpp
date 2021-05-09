#include <gtest/gtest.h>
#include "../../antity/src/core/ECS/Registry.hpp"
using namespace ant;

TEST(Registry,CreateEntity)
{
	Registry reg;
	Entity entityA = reg.CreateEntity();
	Entity entityB = reg.CreateEntity();

	ASSERT_NE(entityA, entityB);
	ASSERT_EQ(entityA, 1);
	
}

TEST(Registry, RegisterComponent)
{
	Registry reg;
	reg.RegisterComponent<int>();
	reg.RegisterComponent<char>();
	
}

TEST(Registry, AddComponent)
{
	Registry reg;

	ChunkID id = 0;
	
	reg.RegisterComponent<int>();
	reg.RegisterComponent<float>();

	Entity entity = reg.CreateEntity();
	
	reg.AddComponent<int>(entity, id, 1);
	reg.AddComponent<float>(entity, id, .4f);
	
}

TEST(Registry, GetComponents)
{
	Registry reg;

	ChunkID id = 0;

	reg.RegisterComponent<int>();
	reg.RegisterComponent<float>();
	reg.RegisterComponent<char>();

	Entity entity = reg.CreateEntity();
	Entity other = reg.CreateEntity();

	reg.AddComponent<int>(entity, id, 1);
	reg.AddComponent<int>(other, id, 10);
	reg.AddComponent<float>(entity, id, .4f);
	reg.AddComponent<char>(other, id, 'c');

	int counter = 0;
	
	for(auto[e,i,f] : reg.GetComponents<int,float>())
	{
		
		counter ++;
		ASSERT_EQ(e, entity);
		ASSERT_EQ(i, 1);
		ASSERT_EQ(f, .4f);
		f = .5f;
	}

	auto [j] = reg.GetEntityComponents<float>(entity);
	ASSERT_EQ(j, .5f);

	ASSERT_EQ(counter, 1);
	
}
