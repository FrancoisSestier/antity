#pragma once
#include "Archetype.hpp"
#include "ArchetypeAllocator.h"
#include "Component.hpp"
#include "Record.hpp"
#include <ranges>

#include "Registry.hpp"

namespace  ant
{
	class ArchetypeHandler
	{
	public:
		ArchetypeHandler(EntityIndex* entityIndex, ComponentMap* componentMap)
			: entityIndex(entityIndex),
			componentMap(componentMap)
		{
		}

		template<typename C, typename ...Args>
		void EmplaceComponent(Entity entity, Archetype* archetype, ComponentBase* component, size_t componentIndex, Args ...args)
		{
			archetypeAllocator.AllocateSpaceIfNeeded(archetype, componentIndex, component);
			C* newComponent
				= new (&archetype->componentArrays[componentIndex].
					componentData[archetype->entities.size() * component->GetSize()])
				C(std::forward<Args>(args)...);
		}


		void MoveLastComponent(Archetype* archetype, size_t toIndex, size_t componentIndex, ComponentBase* component)
		{
			MoveComponent(archetype, archetype, archetype->entities.size() - 1, toIndex, componentIndex, componentIndex, component);
		}

		
		void MoveComponent(Archetype* from, Archetype* to, size_t fromIndex, size_t toIndex, size_t newComponentIndex,size_t oldComponentIndex, ComponentBase* component)
		{
			if (from != to) {
				archetypeAllocator.AllocateSpaceIfNeeded(to, newComponentIndex, component);
			}
			size_t componentSize = component->GetSize();
			component->MoveData(&from->componentArrays[oldComponentIndex].componentData[fromIndex * componentSize],
				&to->componentArrays[newComponentIndex].componentData[toIndex * componentSize]);
		}

		void EraseComponent(Archetype* archetype, const size_t index, size_t ComponentIndex, ComponentBase* component)
		{
			component->DestroyData(&archetype->componentArrays[ComponentIndex].componentData[index * component->GetSize()]);
		}
		
		void CleanArchetype(Archetype* archetype){
			
			for(int j =0; j < archetype->componentArrays.size()){
				auto component = componentMap.at(archetype->archetypeID[i])
				for(int i = 0; i < archetype->entities.size();i++){
					component->DestroyData(&archetype->componentArrays[ComponentIndex].componentData[index * component->GetSize()]);
				}
				delete [] archetype->componentArrays[j].componentData;
				archetype->componentArrats[j].size = 0;
			}			
		}
		
		template<typename ...Cs>
		std::tuple<Cs&...> GetComponents(Archetype* archetype, size_t entityI)
		{
			return (std::tie((GetComponent<Cs>(archetype,entityI),...)));
		}

		template<typename C>
		C& GetComponent(Archetype* archetype, size_t index)
		{
			const size_t componentIndex = std::ranges::find(archetype->archetypeId.begin(), archetype->archetypeId.end(), TypeIdGenerator::GetTypeID<C>()) - archetype->archetypeId.begin();
			return reinterpret_cast<C&>(archetype->componentArrays[componentIndex].componentData[index * sizeof(C)]);
		}
	
	private:
		ArchetypeAllocator archetypeAllocator;
		EntityIndex* entityIndex;
		ComponentMap* componentMap;
	};

}
