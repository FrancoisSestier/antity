#pragma once
#include <algorithm>
#include <memory>
#include "Archetype.h"
#include "antity/utility/robin_hood.h"

namespace ant
{
	class ArchetypeMap
	{
	public:
		using ArchetypeHashTable = robin_hood::unordered_map<
			ArchetypeKey, std::unique_ptr<Archetype>, ArchetypeKey::hasher, ArchetypeKey::comparator>;

		ArchetypeHashTable* get()
		{
			return &archetypeHashTable;
		}

		/**
		 * \brief GetArchetype that match exactly the archetypeKey
		 *		  if no archetype of such type exists creates it
		 * \param archetypeKey 
		 * \return Archetype*
		 */
		Archetype* GetArchetype(const ArchetypeKey& archetypeKey)
		{
			if (!archetypeHashTable.contains(archetypeKey))
			{
				CreateArchetype(archetypeKey);
			}
			return archetypeHashTable.at(archetypeKey).get();
		}

		void DeleteArchetype(const ArchetypeKey& archetypeKey)
		{
			archetypeHashTable.erase(archetypeKey);
		}

		void OnComponentRegistration(ComponentTypeID componentTypeId)
		{
			componentTypeArchetypeMap.emplace(componentTypeId, std::vector<Archetype*>());
		}

		const std::vector<Archetype*>& GetArchetypes(ComponentTypeID componentTypeId)
		{
			return componentTypeArchetypeMap.at(componentTypeId);
		}

		/**
		 * \brief Retrieve all archetypes that have at least all components in givent ArchetpeKey
		 * \param archetypeId
		 * \param chunkId 
		 * \return vector<Archetype*>
		 */
		std::vector<Archetype*> GetArchetypes(ArchetypeID archetypeId, ChunkID chunkId = NULL_CHUNK)
		{

			std::vector<Archetype*> archetypes;
			for (auto&& archetype : archetypeHashTable)
			{
				if (!std::ranges::includes(archetype.second->archetypeId.begin(), archetype.second->archetypeId.end(),
				                           archetypeId.begin(), archetypeId.end()))
				{
					continue;
				}
				if (chunkId != NULL_CHUNK && archetype.second->chunkId != chunkId)
				{
					continue;
				}
				archetypes.emplace_back(archetype.second.get());
			}


			return std::move(archetypes);
		}

	private:
		void CreateArchetype(const ArchetypeKey& archetypeKey)
		{
			auto newArchetype = std::make_unique<Archetype>(archetypeKey.archetypeId, archetypeKey.chunkId);
			for (auto&& componentID : archetypeKey.archetypeId)
			{
				newArchetype->byteArrays.push_back(ByteArray{new std::byte[0], 0});
				AddToComponentArchetypeMap(componentID, newArchetype.get());
			}
			archetypeHashTable.emplace(archetypeKey, std::move(newArchetype));
		}

		void AddToComponentArchetypeMap(ComponentTypeID componentTypeId, Archetype* archetype)
		{
			componentTypeArchetypeMap.at(componentTypeId).emplace_back(archetype);
		}

	private:
		ArchetypeHashTable archetypeHashTable;
		robin_hood::unordered_map<ComponentTypeID, std::vector<Archetype*>> componentTypeArchetypeMap;
	};
}
