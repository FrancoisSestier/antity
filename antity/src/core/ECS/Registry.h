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

		template<typename C, typename ...Args>
		void AddComponent(Entity entity, ChunkID chunkId,Args... args);

		template<typename C>
		void GetComponent();
		
		template<typename ...Cs>
		void RegisterComponents();

		template<typename C>
		void RegisterComponent();
	
	private:
		std::unique_ptr<EntityIndex> entityIndex;
		std::unique_ptr<ComponentMap> componentMap;
		ArchetypeMap archetypeMap;
		UniqueIdDispenser<Entity> entityIdDispenser;
		ArchetypeHandler archetypeHandler;
		RegistryDebugger registryDebugger;
	};

	template <typename C, typename ... Args>
	void Registry::AddComponent(Entity entity,ChunkID chunkId,Args... args)
	{
		if(!entityIndex->contains(entity))
		{
			throw std::runtime_error("Adding Component To Non Existent Entity " + std::to_string(entity));
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
			ArchetypeID oldArchetypeID = oldArchetype->archetypeId;
			ArchetypeID newArchetypeId = oldArchetypeID;
			newArchetypeId.push_back(newComponentTypeId);
			std::ranges::sort(newArchetypeId);
			newArchetype = archetypeMap.GetArchetype(ArchetypeKey{ newArchetypeId,chunkId });
			const size_t oldIndex = record.index;
			size_t LastEntity = oldArchetype->entities.size();
			for(int i = 0; i <newArchetype->componentArrays.size();i++)
			{
				ComponentTypeID componentTypeId = newArchetype->archetypeId.at(i);
				ComponentBase* component = componentMap->at(componentTypeId).get();
				if(componentTypeId == newComponentTypeId)
				{
					archetypeHandler.EmplaceComponent<C>(entity, newArchetype, component,i,std::forward<Args>(args)...);
					continue;
				}
				archetypeHandler.MoveComponent(oldArchetype, newArchetype, oldIndex, i,component);
			}
			archetypeHandler.MoveLastEntityTo(oldArchetype, record.index);
		}

		entityIndex->at(entity).archetype = newArchetype;
		entityIndex->at(entity).index = newArchetype->entities.size();
		newArchetype->entities.push_back(entity);
	}

	template <typename ... Cs>
	void Registry::RegisterComponents()
	{
		(RegisterComponent<Cs>(), ...);
	}

	template <typename C>
	void Registry::RegisterComponent()
	{
		ComponentTypeID componentTypeId = TypeIdGenerator::GetTypeID<C>();
		if(componentMap->contains(componentTypeId))
		{
			throw std::runtime_error("Trying To Register Component Twice");
		}
		componentMap->emplace(componentTypeId, std::unique_ptr<Component<C>>());
		registryDebugger.OnRegisterComponent<C>();
	}
}
