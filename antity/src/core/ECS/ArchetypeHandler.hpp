#pragma once
#include "Archetype.hpp"
#include "ArchetypeAllocator.h"
#include "Component.hpp"
#include "Record.hpp"

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

		void MoveLastEntityTo(Archetype* archetype, size_t entityIndexB)
		{
			size_t lastEntityIndex = archetype->entities.size();
			Entity lastEntity = archetype->entities.at(lastEntityIndex);
			for (int i = 0; i < archetype->componentArrays.size(); i++)
			{
				ComponentTypeID componentTypeId = archetype->archetypeId.at(i);
				ComponentBase* component = componentMap->at(componentTypeId).get();
				component->MoveData(&archetype->componentArrays[i].componentData[lastEntityIndex * component->GetSize()],
					&archetype->componentArrays[i].componentData[entityIndexB * component->GetSize()]);
				component->DestroyData(&archetype->componentArrays[i].componentData[lastEntityIndex * component->GetSize()]);
			}
			archetype->entities.at(entityIndexB) = lastEntity;
			archetype->entities.pop_back();
			entityIndex->at(lastEntity).index = entityIndexB;
		}

		template<typename C, typename ...Args>
		void EmplaceComponent(Entity entity, Archetype* archetype, ComponentBase* component, size_t componentIndex, Args ...args)
		{
			AllocateSpaceIfNeeded(archetype, componentIndex, component);
			C* newComponent
				= new (&archetype->componentArrays[componentIndex].
					componentData[archetype->entities.size() * component->GetSize()])
				C(std::forward<Args>(args)...);
		}


		void MoveComponent(Archetype* from, Archetype* to, size_t oldEntityIndex, size_t newComponentIndex, ComponentBase* component)
		{
			archetypeAllocator.AllocateSpaceIfNeeded(to, newComponentIndex, component);
			size_t componentSize = component->GetSize();
			size_t oldIndex = std::ranges::find(from->archetypeId.begin(), from->archetypeId.end(), component->GetTypeID()) - from->archetypeId.begin();
			component->MoveData(&from->componentArrays[oldIndex].componentData[oldEntityIndex * componentSize],
				&to->componentArrays[newComponentIndex].componentData[to->entities.size() * componentSize]);
		}


	private:
		ArchetypeAllocator archetypeAllocator;
		EntityIndex* entityIndex;
		ComponentMap* componentMap;
	};

}
