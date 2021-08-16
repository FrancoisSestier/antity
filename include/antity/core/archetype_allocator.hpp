#pragma once
#include <antity/core/archetype.hpp>
#include <antity/core/component.hpp>

namespace ant {
    class archetype_allocator {
       public:
        /**
         * \brief allocate memory if needed exponential allocation
         * \param archetype
         * \param component_index
         * \param component
         */
        void auto_allocate(archetype* archetype, size_t component_index,
                           component_base* component) {
            if (needs_space(archetype, component_index, component->get_size())) {
                double_allocation(archetype, component_index, component);
            }
        }

        /**
         * \brief
         * \param archetype Deallocate memory if too much is allocated
         * \param component_index
         * \param component
         */
        void auto_shrink(archetype* archetype, size_t component_index,
                         component_base* component) {
            if (needs_shrinking(archetype, component_index,
                              component->get_size())) {
                shrink_to_fit(archetype, component_index, component);
            }
        }

       private:
        void double_allocation(archetype* archetype, size_t component_index,
                              component_base* component) {
            auto newSize = archetype->byte_arrays.at(component_index).size * 2
                           + component->get_size();
            resize(archetype, component_index, component, newSize);
        }

        void shrink_to_fit(archetype* archetype, size_t component_index,
                         component_base* component) {
            auto newSize = archetype->entities.size() * component->get_size();
            resize(archetype, component_index, component, newSize);
        }

        void resize(archetype* archetype, size_t component_index,
                    component_base* component, size_t newSize) {
            size_t componentSize = component->get_size();
            auto* newData = component->allocate(newSize);
            for (std::size_t e = 0; e < archetype->entities.size(); ++e) {
                component->move_data(&archetype->byte_arrays[component_index]
                                          .componentData[e * componentSize],
                                     &newData[e * componentSize]);
                component->destroy_data(&archetype->byte_arrays[component_index]
                                             .componentData[e * componentSize]);
            }
            delete[] archetype->byte_arrays[component_index].componentData;
            archetype->byte_arrays.at(component_index).size = newSize;
            archetype->byte_arrays[component_index].componentData = newData;
        }

        bool needs_space(archetype* archetype, size_t component_index,
                        size_t componentSize) {
            size_t allocated = archetype->byte_arrays.at(component_index).size;
            size_t needed
                = archetype->entities.size() * componentSize + componentSize;
            return needed > allocated;
        }

        bool needs_shrinking(archetype* archetype, size_t component_index,
                           size_t componentSize) {
            size_t allocated = archetype->byte_arrays.at(component_index).size;
            size_t used = archetype->entities.size() * componentSize;
            return allocated > used / 2;
        }
    };

}  // namespace ant
