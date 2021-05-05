#pragma once
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "Archetype.h"

namespace ant
{
	
	
	class ArchetypeMap
	{
	private:
		typedef std::unordered_set<ArchetypeBase*, ArchetypeBase::hasher, ArchetypeBase::comparator> ArchetypeSet;

		typedef std::unordered_map<Entity, ArchetypeBase*> EntityArchetypeMap;
	
	public:
		~ArchetypeMap()
		{
			for(ArchetypeBase* archetype : archetypeMap)
			{
				delete archetype;
			}
			
		}
		
		template<typename ...Cs>
		Archetype<Cs...>* GetArchetype(ChunkID chunkId = 0)
		{
			ArchetypeID archetypeID = archetype_id<Cs...>().value;
			std::unique_ptr<ArchetypeBase> archetypeBase = std::make_unique<ArchetypeBase>(archetypeID, chunkId);

			if (!archetypeMap.contains(archetypeBase.get()))
			{
				archetypeMap.emplace(new Archetype<Cs...>(chunkId));
			}
			
			return static_cast<Archetype<Cs...>*>(*archetypeMap.find(archetypeBase.get()));
		}

		bool Contains(Entity entity)
		{
			return entityArchetypeMap.contains(entity);
		}

		void OnEntityCreation(Entity entity)
		{
			entityArchetypeMap.emplace(entity,nullptr);
		}
	
	private:
		EntityArchetypeMap entityArchetypeMap;
		ArchetypeSet archetypeMap;
	};


}
