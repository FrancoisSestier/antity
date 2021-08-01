#pragma once
#include <antity/core/archetype.hpp>
#include <antity/core/archetype_allocator.hpp>
#include <antity/core/component.hpp>
#include <antity/core/record.hpp>
#include <antity/core/registry.hpp>
#include <ranges>

namespace ant {
    class archetype_handler {
       public:
        archetype_handler(entity_index* entity_index,
                          component_map* component_map_)
            : entity_index_(entity_index), component_map_(component_map_) {}

        template <typename C, typename... Args>
        void emplace_component(archetype* archetype, component_base* component,
                               size_t componentIndex, Args... args) {
            archetype_allocator_.auto_allocate(archetype, componentIndex,
                                               component);
            C* newComponent
                = new (&archetype->byteArrays[componentIndex]
                            .componentData[archetype->entities.size()
                                           * component->get_size()])
                    C(std::forward<Args>(args)...);
        }

        void move_last_component(archetype* archetype, size_t toIndex,
                                 size_t componentIndex,
                                 component_base* component) {
            move_component(archetype, archetype, archetype->entities.size() - 1,
                           toIndex, componentIndex, componentIndex, component);
        }

        void move_component(archetype* from, archetype* to, size_t fromIndex,
                            size_t toIndex, size_t oldComponentIndex,
                            size_t newComponentIndex,
                            component_base* component) {
            if (from != to) {
                archetype_allocator_.auto_allocate(to, newComponentIndex,
                                                   component);
            }
            size_t componentSize = component->get_size();
            component->mode_data(&from->byteArrays[oldComponentIndex]
                                      .componentData[fromIndex * componentSize],
                                 &to->byteArrays[newComponentIndex]
                                      .componentData[toIndex * componentSize]);
            component->destroy_data(
                &from->byteArrays[oldComponentIndex]
                     .componentData[fromIndex * componentSize]);
        }

        void erase_component(archetype* archetype, const size_t index,
                             size_t ComponentIndex, component_base* component) {
            component->destroy_data(
                &archetype->byteArrays[ComponentIndex]
                     .componentData[index * component->get_size()]);
            archetype_allocator_.auto_shrink(archetype, ComponentIndex,
                                             component);
        }

        /**
         * \brief the only purpose is to be called on registry Dtor. Delete All
         * Components and Deallocate memory from given archetypes DOESN'T clean
         * entities nor their index effectively creating dangling entities;
         * \param archetype
         */
        void clean_archetype_component_arrays(archetype* archetype) {
            for (size_t j = 0; j < archetype->byteArrays.size(); j++) {
                auto component
                    = component_map_->at(archetype->archetype_id[j]).get();
                for (size_t i = 0; i < archetype->entities.size(); i++) {
                    component->destroy_data(
                        &archetype->byteArrays[j]
                             .componentData[i * component->get_size()]);
                }
                delete[] archetype->byteArrays[j].componentData;
                archetype->byteArrays[j].size = 0;
            }
        }

        template <typename... Cs>
        std::tuple<Cs&...> get_components(archetype* archetype,
                                          size_t entityI) {
            return (std::tie((get_component<Cs>(archetype, entityI), ...)));
        }

        template <typename C>
        C& get_component(archetype* archetype, size_t index) {
            const size_t componentIndex
                = std::ranges::find(archetype->archetype_id.begin(),
                                    archetype->archetype_id.end(),
                                    type_id_generator::get<C>())
                  - archetype->archetype_id.begin();
            return *std::launder(
                reinterpret_cast<C*>(&archetype->byteArrays[componentIndex]
                                          .componentData[index * sizeof(C)]));
        }

       private:
        archetype_allocator archetype_allocator_;
        entity_index* entity_index_;
        component_map* component_map_;
    };

}  // namespace ant
