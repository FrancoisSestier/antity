# Antity
Open Source Archetype and Chunk Based Lightweight Entity Component System with straighforward api design

## Motivation
orignally implemented this ecs for my game engine. because i needed chunks to be at the core of the entity component system.
All simillar archetypes divided in as many chunk as you want. GetComponent method can take a ChunkID as parameter. 

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
	Entity entity1 = reg.createEntity<chunkID>();
	Entity entity2 = reg.createEntity<chunkID>();

	//Components
	reg.AddComponents<Position>(entity1,Position{3,4});
	reg.AddComponents<Speed>(entity2,Speed{.1f,.0f});
	
	reg.AddComponents<Position>(entity2,Position{3,4});
	reg.AddComponents<Speed>(entity2,Position{.0f,.2f});

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
## Caveat
the project in still in very early development and not battle tested. obviously do not use it in production.
