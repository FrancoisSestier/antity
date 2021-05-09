#pragma once
#include <stdexcept>
#include <unordered_map>
#include "ArchetypeMap.hpp"
#include "Record.hpp"
#include "../Utility/UniqueIdDispenser.h"
#include <string>

#include "ArchetypeHandler.hpp"
#include "Component.hpp"
#include "RegisteryDebugger.hpp"
#include "../Utility/Iterator.hpp"
#include "View.hpp"

namespace ant {

	class Registry
	{
	public:
		
		Registry()
			: entityIndex(std::make_unique<EntityIndex>()),
			  componentMap(std::make_unique<ComponentMap>()),
			  archetypeHandler(ArchetypeHandler{entityIndex.get(),componentMap.get()})
		{
		}

		/**
		 * \brief Add Entity to the internal indexer
		 * \return created EntityID
		 */
		Entity CreateEntity()
		{
			Entity entity = entityIdDispenser.GetID();
			entityIndex->emplace(entity, Record{ nullptr,0 });
			return entity;
		}

		void RemoveEntity(Entity entity)
		{
			//TODO REMOVE COMPONENTS
			entityIdDispenser.FreeID(entity);
		}

		/**
		 * \brief Add Component To given entity
		 * \tparam C ComponentType
		 * \tparam Args Component Param Type
		 * \param entity
		 * \param chunkId if no chunkID is Given current entity chunkID will be used
		 * \param args Component arguments
		 */
		template<typename C, typename ...Args>
		void AddComponent(Entity entity, ChunkID chunkId = NULL_CHUNK,Args... args);

		template<typename C>
		void RemoveComponent(Entity entity);
		
		template<typename ...Cs>
		void RegisterComponents();

		template<typename C>
		void RegisterComponent();

		/**
		 * \brief builds a multiviews from given components in chunkID
		 * \tparam Cs Component types to be retrieved 
		 * \param chunkid if no chunkID all chunkIDs will be in the multiview
		 * \return a Multiview<Entity,Cs...>
		 */
		template<typename ...Cs>
		MultiView<Entity, Cs...> GetComponents(ChunkID chunkid = NULL_CHUNK);

		template<typename ...Cs>
		std::tuple<Cs&...> GetEntityComponents(Entity entity);
		
		template<typename C>
		View<C> GetComponent();
	private:
	
	private:
		std::unique_ptr<EntityIndex> entityIndex;
		std::unique_ptr<ComponentMap> componentMap;
		ArchetypeMap archetypeMap;
		UniqueIdDispenser<Entity> entityIdDispenser;
		ArchetypeHandler archetypeHandler;
		RegistryDebugger registryDebugger;
		ArchetypeViewBuilder archetypeViewBuilder;
	};


	template <typename C, typename ... Args>
	void Registry::AddComponent(Entity entity,ChunkID chunkId,Args... args)
	{
		if(!entityIndex->contains(entity))
		{
			RegisterComponent<C>();
		}

		const Record record = entityIndex->at(entity);
		
		Archetype* oldArchetype = record.archetype;
		Archetype* newArchetype;

		ComponentTypeID newComponentTypeId = TypeIdGenerator::GetTypeID<C>();
		
		if (oldArchetype == nullptr)
		{
			newArchetype = archetypeMap.GetArchetype(
				ArchetypeKey{ ArchetypeID{newComponentTypeId},chunkId });
			ComponentBase* component = componentMap->at(newComponentTypeId).get();
			archetypeHandler.EmplaceComponent<C>(entity, newArchetype, component, 0, std::forward<Args>(args)...);
		} else
		{
			if(chunkId == NULL_CHUNK)
			{
				chunkId = oldArchetype->chunkId;
			}
			ArchetypeID oldArchetypeID = oldArchetype->archetypeId;
			ArchetypeID newArchetypeId = oldArchetypeID;
			newArchetypeId.push_back(newComponentTypeId);
			std::ranges::sort(newArchetypeId);
			newArchetype = archetypeMap.GetArchetype(ArchetypeKey{ newArchetypeId,chunkId });
			const size_t oldIndex = record.index;
			const size_t LastEntityIndex = oldArchetype->entities.size()-1;
			int oldCompIndex = 0;
			for(int i = 0; i <newArchetype->componentArrays.size();i++)
			{
				ComponentTypeID componentTypeId = newArchetype->archetypeId.at(i);
				ComponentBase* component = componentMap->at(componentTypeId).get();
				if(componentTypeId == newComponentTypeId)
				{
					archetypeHandler.EmplaceComponent<C>(entity, newArchetype, component,i,std::forward<Args>(args)...);
					continue;
				}
				archetypeHandler.MoveComponent(oldArchetype, newArchetype, oldIndex, newArchetype->entities.size(),i , oldCompIndex, component);
				if (LastEntityIndex != oldIndex) {
					archetypeHandler.MoveLastComponent(oldArchetype, oldIndex, oldCompIndex, component);
				}
				archetypeHandler.EraseComponent(oldArchetype, LastEntityIndex,oldCompIndex, component);
				oldCompIndex ++ ;
			}
			if (LastEntityIndex != oldIndex) {
				const Entity lastEntity = oldArchetype->entities.at(LastEntityIndex);
				oldArchetype->entities.at(oldIndex) = lastEntity;
				entityIndex->at(lastEntity).index = oldIndex;
			}
			oldArchetype->entities.pop_back();
				
		}

		entityIndex->at(entity).archetype = newArchetype;
		entityIndex->at(entity).index = newArchetype->entities.size();
		newArchetype->entities.push_back(entity);
	}

	template <typename C>
	void Registry::RemoveComponent(Entity entity)
	{
		const Record record = entityIndex->at(entity);
		/*
		Archetype* oldArchetype = record.archetype;
		Archetype* newArchetype;
		ComponentTypeID componentToRemoveTypeId = TypeIdGenerator::GetTypeID<C>();
		ArchetypeID newArchetypeID = oldArchetype->archetypeId;
		auto componentToRemove = std::ranges::find(newArchetypeID.begin(), newArchetypeID.end(), componentToRemoveTypeId);
		size_t componentToRemoveIndex = componentToRemove - newArchetypeID.begin();
		newArchetypeID.erase(componentToRemove);
		const size_t oldIndex = record.index;
		const size_t LastEntityIndex = oldArchetype->entities.size() - 1;
		if(newArchetypeID.size() != 0)
		{
			newArchetype = archetypeMap.GetArchetype(ArchetypeKey{ newArchetypeID, oldArchetype->chunkId });
			int c = 0;
			for (int i = 0; i < oldArchetype->componentArrays.size(); i++)
			{
				if(i != componentToRemoveIndex)
				{
					auto comp = componentMap->at(oldArchetype->archetypeId.at(i)).get();
					archetypeHandler.MoveComponent(oldArchetype, newArchetype, oldIndex, c, comp);
					if (LastEntityIndex != oldIndex) {
						archetypeHandler.(oldArchetype, oldArchetype, LastEntityIndex, oldIndex, component);
					}
					archetypeHandler.EraseComponent(oldArchetype, LastEntityIndex, i, component);
					c++;
				}
				
			}
		}
		*/
		

		
	}

	template <typename C>
	View<C> Registry::GetComponent()
	{
		return archetypeViewBuilder.BuildComponentView<C>(archetypeMap.GetArchetypes(TypeIdGenerator::GetTypeID<C>()));
	}

	template <typename ... Cs>
	void Registry::RegisterComponents()
	{
		(RegisterComponent<Cs>(), ...);
	}

	template <typename C>
	void Registry::RegisterComponent()
	{
		ComponentTypeID componentTypeId = static_cast<ComponentTypeID>(TypeIdGenerator::GetTypeID<C>());
		if(componentMap->contains(componentTypeId))
		{
			throw std::runtime_error("Trying To Register Component Twice");
		}
		componentMap->emplace(componentTypeId, std::make_unique<Component<C>>());
		registryDebugger.OnComponentRegistration<C>();
		archetypeMap.OnComponentRegistration(componentTypeId);
	}

	template <typename ... Cs>
	MultiView<Entity, Cs...> Registry::GetComponents(ChunkID chunkID)
	{
		ArchetypeID archetypeId = ArchetypeID{ static_cast<ComponentTypeID>(TypeIdGenerator::GetTypeID<Cs>()) ... };
		std::ranges::sort(archetypeId);
		const auto archetypes = archetypeMap.GetArchetypes(archetypeId, chunkID);
		return archetypeViewBuilder.BuildMultiComponentView<Cs...>(archetypes);
	}

	template <typename ... Cs>
	std::tuple<Cs&...> Registry::GetEntityComponents(Entity entity)
	{
		return archetypeHandler.GetComponents<Cs...>(entityIndex->at(entity).archetype, entityIndex->at(entity).index);
	}
}
