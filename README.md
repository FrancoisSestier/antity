# Antity [![Windows](https://github.com/FrancoisSestier/antity/actions/workflows/windows.yml/badge.svg)](https://github.com/FrancoisSestier/antity/actions/workflows/windows.yml) [![Windows](https://github.com/FrancoisSestier/antity/actions/workflows/windows.yml/badge.svg)](https://github.com/FrancoisSestier/antity/actions/workflows/windows.yml)
Open Source Archetype and Chunk Based Lightweight Entity Component System with straighforward api design

## Motivation
orignally implemented this ecs for my game engine. because i needed chunks to be at the core of the entity component system.

## ECS Design
The Design is based on archetypes. Archetypes are a collection of Component types. 
All simillar Archetypes can be divided in as many chunks as you want. GetComponent method can take a ChunkID as parameter. 
Two entities that have the same collection of components and the same chunkID will be stored in the same archetype. 
All entities in the same archetype are guarenteed to have their components stored in contiguous arrays. 

## Usage Design 
The API is as simple as it can be :

```c++
struct Position {
	int x;
	int y;
};

struct Speed {
	float x;
	float x;
};


typedef uint32_t ChunkID;

int main(){


	reg Registry;

	//Entities
	Entity entity1 = reg.createEntity();
	Entity entity2 = reg.createEntity();

	//Components
	reg.AddComponents<Position>(entity1,{3,4});
	reg.AddComponents<Speed>(entity2,{.1f,.0f});
	
	reg.AddComponents<Position>(entity2,{3,4});
	reg.AddComponents<Speed>(entity2,Posiion{.0f,.2f});

	//Systems
	const auto deltaTime = .001f
	while(true){
		for(auto [entity, pos ,speed] : reg.GetComponents<Position,Speed>){
			pos.x += speed.x * deltaTime;		
			pos.y += speed.y * deltaTime;		

		}
	}

}
```

## What's forbidden
For a given Component of type C, the following must be true alignof(C) <= sizeof(C). (i'm looking at a way to fix this)
During iteration over a View of component you can neither add components nor remove components.
Not following those rules will result in undefined behavior !

## Caveat
the project in still in very early development and not battle tested. obviously do not use it in production.
