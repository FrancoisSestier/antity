#pragma once
#include "Archetype.h"
#include "ArchetypeAllocator.h"
#include "Component.h"
#include "Record.h"
#include <ranges>

#include "Registry.h"

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
		void EmplaceComponent(Archetype* archetype, ComponentBase* component, size_t componentIndex, Args ...args)
		{
			archetypeAllocator.AutoAllocate(archetype, componentIndex, component);
			C* newComponent
					= new (&archetype->byteArrays[componentIndex].
						componentData[archetype->entities.size() * component->GetSize()])
					C(std::forward<Args>(args)...);
		}

		void MoveLastComponent(Archetype* archetype, size_t toIndex, size_t componentIndex, ComponentBase* component)
		{
			MoveComponent(archetype, archetype, archetype->entities.size() - 1, toIndex, componentIndex, componentIndex, component);
		}

		
		void MoveComponent(Archetype* from, Archetype* to, size_t fromIndex, size_t toIndex, size_t oldComponentIndex, size_t newComponentIndex, ComponentBase* component)
		{
			if (from != to) {
				archetypeAllocator.AutoAllocate(to, newComponentIndex, component);
			}
			size_t componentSize = component->GetSize();
			component->MoveData(&from->byteArrays[oldComponentIndex].componentData[fromIndex * componentSize],
				&to->byteArrays[newComponentIndex].componentData[toIndex * componentSize]);
			component->DestroyData(&from->byteArrays[oldComponentIndex].componentData[fromIndex * componentSize]);
		}

		void EraseComponent(Archetype* archetype, const size_t index, size_t ComponentIndex, ComponentBase* component)
		{
			component->DestroyData(&archetype->byteArrays[ComponentIndex].componentData[index * component->GetSize()]);
			archetypeAllocator.AutoShrink(archetype, ComponentIndex, component);
		}
		
		/**
		 * \brief the only purpose is to be called on registry Dtor. Delete All Components and Deallocate memory from given archetypes
		 *  DOESN'T clean entities nor their index effectively creating dangling entities;
		 * \param archetype 
		 */
		void CleanArchetypeComponentArrays(Archetype* archetype){
			
			for (int j = 0; j < archetype->byteArrays.size();j++) {
				auto component = componentMap->at(archetype->archetypeId[j]).get();
				for(int i = 0; i < archetype->entities.size();i++){
					component->DestroyData(&archetype->byteArrays[j].componentData[i * component->GetSize()]);
				}
				delete [] archetype->byteArrays[j].componentData;
				archetype->byteArrays[j].size = 0;
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
			return *std::launder(reinterpret_cast<C*>(&archetype->byteArrays[componentIndex].componentData[index * sizeof(C)]));
		}
	
	private:
		ArchetypeAllocator archetypeAllocator;
		EntityIndex* entityIndex;
		ComponentMap* componentMap;
	};

}
