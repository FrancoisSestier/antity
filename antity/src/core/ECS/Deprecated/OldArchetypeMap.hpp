#pragma once
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "Archetype.h"

namespace ant
{
	struct Record
	{
		ArchetypeBase* archetypeBase;
		size_t index;
	};

	typedef std::unordered_map<Entity, Record> EntityRecordsMap;
	
	class OldArchetypeMap
	{
	private:
		typedef std::unordered_set<ArchetypeBase*, ArchetypeBase::hasher, ArchetypeBase::comparator> ArchetypeSet;
	
	public:
		ArchetypeMap(EntityRecordsMap* entityRecordsMap) : entityRecordsMap(entityRecordsMap) {}
		
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
	
	private:
		EntityRecordsMap* entityRecordsMap;
		ArchetypeSet archetypeMap;
	};


}
