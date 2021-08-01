[![Windows](https://github.com/FrancoisSestier/antity/actions/workflows/windows.yml/badge.svg)](https://github.com/FrancoisSestier/antity/actions/workflows/windows.yml) [![Ubuntu](https://github.com/FrancoisSestier/antity/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/FrancoisSestier/antity/actions/workflows/ubuntu.yml) [![codecov](https://codecov.io/gh/FrancoisSestier/antity/branch/master/graph/badge.svg?token=ZPDP1TAO3Z)](https://codecov.io/gh/FrancoisSestier/antity) [![License: Unlicense](https://img.shields.io/badge/license-Unlicense-blue.svg)](http://unlicense.org/)

<p align="center">
  <img src="https://user-images.githubusercontent.com/17357315/119251805-ce998700-bba8-11eb-8150-f642ca16cab8.png" height="175" width="auto" />
</p>

Open Source archetype and Chunk Based Lightweight entity_t Component System with straighforward api design

## Motivation
orignally implemented this ecs for my game engine. because i needed chunks to be at the core of the entity_t component system.

## ECS Design
The Design is based on archetypes. archetypes are a collection of Component types.  
All simillar archetypes can be divided in as many chunks as you want. get_component method can take a chunk_id_t as parameter.  
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


int main(){


	reg Registry;

	//Entities
	entity_t entity1 = reg.create();
	entity_t entity2 = reg.create();

	//Components
	reg.add<Position>(entity1,{3,4});
	reg.add<Speed>(entity1,{.1f,.0f});
	
	reg.add<Position>(entity2,{3,4});
	reg.add<Speed>(entity2,{.0f,.2f});

	//Systems
	const auto deltaTime = .001f
	for(auto [entity, pos ,speed] : reg.get<Position,Speed>){
		pos.x += speed.x * deltaTime;		
		pos.y += speed.y * deltaTime;		

	}

}
```

## What's forbidden 
During iteration over a View of component you can neither add components nor remove components.  
Not following those rules will result in undefined behavior !  

## Caveat
the project in still in very early development and not battle tested. obviously do not use it in production.  

## Thanks
https://github.com/skypjack/entt by spyjack and he's ECS back and forth series.  
https://github.com/SanderMertens/flecs by sander mertens and he's building and ECS series.  
https://indiegamedev.net/2020/05/19/an-entity_t-component-system-with-data-locality-in-cpp/ by DeckHead.  
 
