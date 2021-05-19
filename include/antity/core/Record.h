#pragma once
#include "Archetype.h"

namespace ant {

	struct Record
	{
		Archetype* archetype = nullptr;
		size_t index = 0;
		ChunkID chunkId = 0;
	};

	using EntityIndex = std::unordered_map < Entity, Record>;

}