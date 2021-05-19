#pragma once
#include <stdexcept>
#include <unordered_map>
#include "ArchetypeMap.h"
#include "Record.h"
#include "../utility/UniqueIdDispenser.h"
#include <string>

#include "ArchetypeHandler.h"
#include "Component.h"
#include "RegisteryDebugger.h"
#include "../utility/Iterator.h"
#include "View.h"

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

		~Registry()
		{
			for(auto&& it : *archetypeMap.get())
			{
				archetypeHandler.CleanArchetypeComponentArrays(it.second.get());
			}
		}
		
		/**
		 * \brief Add Entity to the internal indexer
		 * \return created EntityID
		 */
		Entity CreateEntity(ChunkID chunkId = 0)
		{
			Entity entity = entityIdDispenser.GetID();
			entityIndex->emplace(entity, Record{ nullptr,0,chunkId });
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
		void AddComponent(Entity entity,Args&& ...args);

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

		/**
		 * \brief Get all given Component from an entity
		 * \tparam Cs ComponentTypes to retrieve
		 * \param entity 
		 * \return tuple of component refereces ;
		 */
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
	void Registry::AddComponent(Entity entity,Args&& ...args)
	{
		if(!entityIndex->contains(entity))
		{
			throw std::runtime_error("unregisterd entity");
			
		}
		ComponentTypeID newComponentTypeId = TypeIdGenerator::GetTypeID<C>();

		if(!componentMap->contains(newComponentTypeId))
		{
			RegisterComponent<C>();
		}

		const Record record = entityIndex->at(entity);
		
		Archetype* oldArchetype = record.archetype;
		Archetype* newArchetype;

		
		if (oldArchetype == nullptr)
		{
			newArchetype = archetypeMap.GetArchetype(
				ArchetypeKey{ ArchetypeID{newComponentTypeId},record.chunkId });
			ComponentBase* component = componentMap->at(newComponentTypeId).get();
			archetypeHandler.EmplaceComponent<C>(newArchetype, component, 0, std::forward<Args>(args)...);
		} else
		{
			ArchetypeID oldArchetypeID = oldArchetype->archetypeId;
			ArchetypeID newArchetypeId = oldArchetypeID;
			newArchetypeId.push_back(newComponentTypeId);
			std::ranges::sort(newArchetypeId);
			newArchetype = archetypeMap.GetArchetype(ArchetypeKey{ newArchetypeId,record.chunkId });
			const size_t oldIndex = record.index;
			const size_t LastEntityIndex = oldArchetype->entities.size()-1;
			int oldCompIndex = 0;
			for(int i = 0; i <newArchetype->componentArrays.size();i++)
			{
				ComponentTypeID componentTypeId = newArchetype->archetypeId.at(i);
				ComponentBase* component = componentMap->at(componentTypeId).get();
				if(componentTypeId == newComponentTypeId)
				{
					archetypeHandler.EmplaceComponent<C>(newArchetype, component,i,std::forward<Args>(args)...);
					continue;
				}
				archetypeHandler.MoveComponent(oldArchetype, newArchetype, oldIndex, newArchetype->entities.size(),oldCompIndex , i, component);
				if (LastEntityIndex != oldIndex) {
					archetypeHandler.MoveLastComponent(oldArchetype, oldIndex, oldCompIndex, component);
				}
				oldCompIndex ++ ;
			}
			if (LastEntityIndex != oldIndex) {
				const Entity lastEntity = oldArchetype->entities.at(LastEntityIndex);
				oldArchetype->entities.at(oldIndex) = lastEntity;
				entityIndex->at(lastEntity).index = oldIndex;
			}
			oldArchetype->entities.pop_back();
			if(oldArchetype->entities.empty())
			{
				archetypeHandler.CleanArchetypeComponentArrays(oldArchetype);
				archetypeMap.DeleteArchetype(ArchetypeKey{ oldArchetype->archetypeId,oldArchetype->chunkId });
			}
				
		}

		entityIndex->at(entity).archetype = newArchetype;
		entityIndex->at(entity).index = newArchetype->entities.size();
		newArchetype->entities.push_back(entity);
	}

	template <typename C>
	void Registry::RemoveComponent(Entity entity)
	{
		const Record record = entityIndex->at(entity);
		
		Archetype* oldArchetype = record.archetype;
		Archetype* newArchetype = nullptr;
		ComponentTypeID componentToRemoveTypeId = TypeIdGenerator::GetTypeID<C>();
		ArchetypeID newArchetypeID = oldArchetype->archetypeId;
		auto componentToRemove = std::ranges::find(newArchetypeID.begin(), newArchetypeID.end(), componentToRemoveTypeId);
		size_t componentToRemoveIndex = componentToRemove - newArchetypeID.begin();
		newArchetypeID.erase(componentToRemove);
		const size_t oldIndex = record.index;
		const size_t LastEntityIndex = oldArchetype->entities.size() - 1;
		if (newArchetypeID.size() != 0)
		{
			newArchetype = archetypeMap.GetArchetype(ArchetypeKey{ newArchetypeID, oldArchetype->chunkId });
			entityIndex->at(entity).archetype = newArchetype;
			entityIndex->at(entity).index = newArchetype->entities.size();
			newArchetype->entities.push_back(entity);
		}
		int c = 0;
		for (int i = 0; i < oldArchetype->componentArrays.size(); i++)
		{
			ComponentBase* component = componentMap->at(oldArchetype->archetypeId.at(i)).get();
			if(i != componentToRemoveIndex && newArchetypeID.size() != 0)
			{
				if(newArchetype != nullptr){
                            archetypeHandler.MoveComponent(oldArchetype, newArchetype, oldIndex,
                                                           newArchetype->entities.size() - 1, i, c,
                                                           component);
				}
				c++;
			} else
			{
				archetypeHandler.EraseComponent(oldArchetype, oldIndex, i, component);
			}
			if (LastEntityIndex != oldIndex) {
				archetypeHandler.MoveLastComponent(oldArchetype, oldIndex, i, component);
			}
		}
		if (LastEntityIndex != oldIndex) {
			const Entity lastEntity = oldArchetype->entities.at(LastEntityIndex);
			oldArchetype->entities.at(oldIndex) = lastEntity;
			entityIndex->at(lastEntity).index = oldIndex;
		}
		oldArchetype->entities.pop_back();

		if(oldArchetype->entities.empty())
		{
			archetypeHandler.CleanArchetypeComponentArrays(oldArchetype);
			archetypeMap.DeleteArchetype(ArchetypeKey{ oldArchetype->archetypeId,oldArchetype->chunkId });
		}
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
		archetypeMap.OnComponentRegistration(TypeIdGenerator::GetTypeID<C>());
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