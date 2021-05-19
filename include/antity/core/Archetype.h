#pragma once
#include "Identifier.h"
#include <cstddef>
#include "../utility/Hasher.h"

namespace  ant
{
	struct  Archetype;

	typedef std::byte* ComponentData;

	struct ComponentArray
	{
		ComponentData componentData;
		size_t size;
	};

	struct ArchetypeKey
	{
		ArchetypeID archetypeId;
		ChunkID chunkId;

		struct hasher
		{
			size_t operator()(const ArchetypeKey& key) const
			{
				return hash::hash_combine(hash::hash(key.archetypeId), hash::hash(key.chunkId));
			}
		};

		struct comparator
		{
			bool operator()(const ArchetypeKey& lhs, const ArchetypeKey& rhs) const
			{
				if (lhs.archetypeId.size() != rhs.archetypeId.size() || lhs.chunkId != rhs.chunkId)
				{
					return false;
				}

				for (size_t i = 0; i < lhs.archetypeId.size(); i++)
				{
					if (lhs.archetypeId[i] != rhs.archetypeId[i])
					{
						return false;
					}
				}

				return true;
			}
		};
		
	};
	
	struct Archetype
	{
		ArchetypeID archetypeId;
		ChunkID chunkId;
		std::vector<ComponentArray> componentArrays;
		std::vector<Entity> entities;
	};

}