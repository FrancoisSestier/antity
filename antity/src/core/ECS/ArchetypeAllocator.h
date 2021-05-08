#pragma once
#include "Archetype.hpp"
#include "Component.hpp"

namespace ant
{
	class ArchetypeAllocator {
	public:
		void AllocateSpaceIfNeeded(Archetype* archetype, size_t componentIndex, ComponentBase* component)
		{
			if (NeedSpace(archetype, componentIndex, component->GetSize()))
			{
				DoubleAllocation(archetype, componentIndex, component);
			}
		}
		
	private:
		void DoubleAllocation(Archetype* archetype, size_t componentIndex, ComponentBase* component)
		{
			size_t size = component->GetSize();
			archetype->componentArrays.at(componentIndex).size = archetype->componentArrays.at(componentIndex).size * 2 + component->GetSize();
			auto* newData = new std::byte[archetype->componentArrays.at(componentIndex).size];
			for (std::size_t e = 0; e < archetype->entities.size(); ++e)
			{
				component->MoveData(&archetype->componentArrays[componentIndex].componentData[e * size],
					&newData[e * size]);
				component->DestroyData(&archetype->componentArrays[componentIndex].componentData[e * size]);
			}
			delete[] archetype->componentArrays[componentIndex].componentData;

			archetype->componentArrays[componentIndex].componentData = newData;
		}

		bool NeedSpace(Archetype* archetype, size_t componentIndex, size_t componentSize)
		{
			size_t currentSize = archetype->componentArrays.at(componentIndex).size;
			size_t newSize = archetype->entities.size() * componentSize;
			return newSize > currentSize;
		}

	};
	
}
