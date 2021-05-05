Usage Goal : 

```c++
struct Position {
	int x;
	int y;
};

struct Speed {
	float x;
};


typedef uint32_t ChunkID;

main(){

	ChunkID chunkId = 1;

	reg Registry;

	reg.registerComponent<Position>;
	reg.registerComponent<Speed>;

	Entity entity1 = reg.createEntity<chunkID>();
	Entity entity2 = reg.createEntity<chunkID>();

	reg.AddComponents<Position,Speed>(entity,Position{1,1},Speed{.1,.1));
	reg.AddComponents<Position,Speed>(entity,Position{2,2},Speed{.1,.1));

	for(auto [pos,speed] : reg.GetComponents<Position,Speed>){
	
		pos += speed * deltaTime;		

	}


}
```
