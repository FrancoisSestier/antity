#pragma once
#include "Archetype.h"
#include "antity/utility/robin_hood.h"
#include "Identifier.h"

namespace ant {

	struct Record
	{
		Archetype* archetype = nullptr;
		IDType index = 0;
		ChunkID chunkId = 0;
	};

	using EntityIndex = robin_hood::unordered_map < Entity, Record>;

}