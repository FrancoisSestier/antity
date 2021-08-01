#pragma once
#include <antity/core/archetype.hpp>
#include <antity/core/component.hpp>

namespace ant {
    class archetype_allocator {
       public:
        /**
         * \brief allocate memory if needed exponential allocation
         * \param archetype
         * \param componentIndex
         * \param component
         */
        void auto_allocate(archetype* archetype, size_t componentIndex,
                           component_base* component) {
            if (NeedsSpace(archetype, componentIndex, component->get_size())) {
                DoubleAllocation(archetype, componentIndex, component);
            }
        }

        /**
         * \brief
         * \param archetype Deallocate memory if too much is allocated
         * \param componentIndex
         * \param component
         */
        void auto_shrink(archetype* archetype, size_t componentIndex,
                         component_base* component) {
            if (NeedsShriking(archetype, componentIndex,
                              component->get_size())) {
                ShrinkToFit(archetype, componentIndex, component);
            }
        }

       private:
        void DoubleAllocation(archetype* archetype, size_t componentIndex,
                              component_base* component) {
            auto newSize = archetype->byteArrays.at(componentIndex).size * 2
                           + component->get_size();
            Resize(archetype, componentIndex, component, newSize);
        }

        void ShrinkToFit(archetype* archetype, size_t componentIndex,
                         component_base* component) {
            auto newSize = archetype->entities.size() * component->get_size();
            Resize(archetype, componentIndex, component, newSize);
        }

        void Resize(archetype* archetype, size_t componentIndex,
                    component_base* component, size_t newSize) {
            size_t componentSize = component->get_size();
            auto* newData = component->allocate(newSize);
            for (std::size_t e = 0; e < archetype->entities.size(); ++e) {
                component->mode_data(&archetype->byteArrays[componentIndex]
                                          .componentData[e * componentSize],
                                     &newData[e * componentSize]);
                component->destroy_data(&archetype->byteArrays[componentIndex]
                                             .componentData[e * componentSize]);
            }
            delete[] archetype->byteArrays[componentIndex].componentData;
            archetype->byteArrays.at(componentIndex).size = newSize;
            archetype->byteArrays[componentIndex].componentData = newData;
        }

        bool NeedsSpace(archetype* archetype, size_t componentIndex,
                        size_t componentSize) {
            size_t allocated = archetype->byteArrays.at(componentIndex).size;
            size_t needed
                = archetype->entities.size() * componentSize + componentSize;
            return needed > allocated;
        }

        bool NeedsShriking(archetype* archetype, size_t componentIndex,
                           size_t componentSize) {
            size_t allocated = archetype->byteArrays.at(componentIndex).size;
            size_t used = archetype->entities.size() * componentSize;
            return allocated > used / 2;
        }
    };

}  // namespace ant
