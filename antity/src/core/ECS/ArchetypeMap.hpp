#pragma once
#include <memory>
#include <unordered_map>

#include "Archetype.hpp"

namespace ant {

	class ArchetypeMap
	{
	public:
		Archetype* GetArchetype(const ArchetypeKey& archetypeKey)
		{
			if(!archetypeHashTable.contains(archetypeKey))
			{
				CreateArchetype(archetypeKey);
			}
			return archetypeHashTable.at(archetypeKey).get();
		}
	
	private:
		void CreateArchetype(const ArchetypeKey& archetypeKey)
		{
			std::unique_ptr<Archetype> newArchetype = std::make_unique<Archetype>(archetypeKey.archetypeId, archetypeKey.chunkId);
			for (auto&& componentID : archetypeKey.archetypeId)
			{
				newArchetype->componentArrays.push_back(ComponentArray{ new std::byte[0],0 });
			}
			archetypeHashTable.emplace(archetypeKey, std::move(newArchetype));
		}
	
	private:
		std::unordered_map<ArchetypeKey,std::unique_ptr<Archetype>,ArchetypeKey::hasher,ArchetypeKey::comparator> archetypeHashTable;
	};

}