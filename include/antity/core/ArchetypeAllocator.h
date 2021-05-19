#pragma once
#include "Archetype.h"
#include "Component.h"

namespace ant
{
	class ArchetypeAllocator {
	public:

		
		/**
		 * \brief Allocate memory if needed exponential allocation
		 * \param archetype 
		 * \param componentIndex 
		 * \param component 
		 */
		void AutoAllocate(Archetype* archetype, size_t componentIndex, ComponentBase* component)
		{
			if (NeedsSpace(archetype, componentIndex, component->GetSize()))
			{
				DoubleAllocation(archetype, componentIndex, component);
			}
		}

		/**
		 * \brief 
		 * \param archetype DeAllocate memory if too much is allocated
		 * \param componentIndex 
		 * \param component 
		 */
		void AutoShrink(Archetype* archetype, size_t componentIndex, ComponentBase* component)
		{
			if (NeedsShriking(archetype, componentIndex, component->GetSize()))
			{
				ShrinkToFit(archetype, componentIndex, component);
			}
		}
		
	private:
		
		void DoubleAllocation(Archetype* archetype, size_t componentIndex, ComponentBase* component)
		{
			auto newSize = archetype->componentArrays.at(componentIndex).size * 2 + component->GetSize();
			Resize(archetype, componentIndex, component, newSize);
		}

		void ShrinkToFit(Archetype* archetype, size_t componentIndex, ComponentBase* component)
		{
			auto newSize = archetype->entities.size() * component->GetSize();
			Resize(archetype, componentIndex, component, newSize);
		}

		void Resize(Archetype* archetype, size_t componentIndex, ComponentBase* component,size_t newSize)
		{
			size_t componentSize = component->GetSize();
			auto* newData = component->Allocate(newSize);
			for (std::size_t e = 0; e < archetype->entities.size(); ++e)
			{
				component->MoveData(&archetype->componentArrays[componentIndex].componentData[e * componentSize],
					&newData[e * componentSize]);
				component->DestroyData(&archetype->componentArrays[componentIndex].componentData[e * componentSize]);
			}
			delete[] archetype->componentArrays[componentIndex].componentData;
			archetype->componentArrays.at(componentIndex).size = newSize;
			archetype->componentArrays[componentIndex].componentData = newData;
		}
		
		bool NeedsSpace(Archetype* archetype, size_t componentIndex, size_t componentSize)
		{
			size_t allocated = archetype->componentArrays.at(componentIndex).size;
			size_t needed = archetype->entities.size() * componentSize + componentSize;
			return needed > allocated;
		}

		bool NeedsShriking(Archetype* archetype, size_t componentIndex, size_t componentSize)
		{
			size_t allocated = archetype->componentArrays.at(componentIndex).size;
			size_t used = archetype->entities.size() * componentSize;
			return allocated > used/2;
		}

	};
	
}
