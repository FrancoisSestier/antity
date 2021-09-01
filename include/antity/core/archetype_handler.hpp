#pragma once
#include <antity/core/archetype.hpp>
#include <antity/core/archetype_allocator.hpp>
#include <antity/core/component.hpp>
#include <antity/core/record.hpp>
#include <antity/core/registry.hpp>
#include <antity/utility/function_traits.hpp>
#include <ranges>

namespace ant {
    class archetype_handler {
       public:
        archetype_handler(entity_index* entity_index,
                          component_map* component_map_)
            : entity_index_(entity_index), component_map_(component_map_) {}

        /**
         * @brief emplacing component in the given archetype's byte_arrays
         *
         * @tparam C type of the component being created
         * @tparam Args arguments to create the component of type C
         * @param archetype archetype the component has to be emplaced in
         * @param component_index index of the component being created in the
         * byte_arrays
         * @param args arguments that are forwared to component Ctor
         */
        template <typename C, typename... Args>
        void emplace_component(archetype* archetype, size_t component_index,
                               Args... args) {
            archetype_allocator_.auto_allocate(
                archetype, component_index,
                component_map_->at(type_id_generator::get<C>()).get());
            C* newComponent = new (
                &archetype->byte_arrays[component_index]
                     .componentData[archetype->entities.size() * sizeof(C)])
                C(std::forward<Args>(args)...);
        }

        template <typename C>
        void insert_component(archetype* archetype, C&& c) {
            size_t component_index{
                archetype->component_id_index.at(type_id_generator::get<C>())};
            archetype_allocator_.auto_allocate(
                archetype, component_index,
                component_map_->at(type_id_generator::get<C>()).get());
            C* newComponent = new (
                &archetype->byte_arrays[component_index]
                     .componentData[archetype->entities.size() * sizeof(C)])
                C(std::forward<C>(c));
        }

        /**
         * @brief move last component of given archetype
         *
         * @param archetype ptr to the archetype containing the component that
         * needs to be moved
         * @param to_index (entity) index that the component has to be moved to
         * @param component_index index of the component type in the byte_arrays
         * @param component ptr to the base_component being moved
         */
        void move_last_component(archetype* archetype, size_t to_index,
                                 size_t component_index,
                                 component_base* component) {
            move_component(archetype, archetype, archetype->entities.size() - 1,
                           to_index, component_index, component_index,
                           component);
        }

        /**
         * @brief move component from one archetype to another
         *
         * @param from ptr to the archetype currently holding the component
         * @param to ptr to the archetype component has to be moved to
         * @param from_index index of the entity containing the component in the
         * old archetype
         * @param to_index index of that the entity will be moved to in the new
         * archetype
         * @param old_component_index index of the component_type in the old
         * archetype byte_arrays
         * @param new_component_index index of the component_type in the new
         * archetype byte_arrays
         * @param component ptr to the base_component being moved
         */
        void move_component(archetype* from, archetype* to, size_t from_index,
                            size_t to_index, size_t old_component_index,
                            size_t new_component_index,
                            component_base* component) {
            if (from != to) {
                archetype_allocator_.auto_allocate(to, new_component_index,
                                                   component);
            }
            size_t componentSize = component->get_size();
            component->move_data(
                &from->byte_arrays[old_component_index]
                     .componentData[from_index * componentSize],
                &to->byte_arrays[new_component_index]
                     .componentData[to_index * componentSize]);
            component->destroy_data(
                &from->byte_arrays[old_component_index]
                     .componentData[from_index * componentSize]);
        }

        /**
         * @brief erase component of given archetype and entity index
         * effectively calling dtor shrinking component arrays if necessary
         *
         * @param archetype ptr to the archetype containing the component to
         * delete
         * @param index the index of the entity that component needs to be
         * removed
         * @param component_index index of the component type in the byte_arrays
         * @param component ptr to the component_base needed to call correct
         * dtor
         */
        void erase_component(archetype* archetype, const size_t index,
                             size_t component_index,
                             component_base* component) {
            component->destroy_data(
                &archetype->byte_arrays[component_index]
                     .componentData[index * component->get_size()]);
            archetype_allocator_.auto_shrink(archetype, component_index,
                                             component);
        }

        /**
         * \brief the only purpose is to be called on registry Dtor. Delete All
         * Components and Deallocate memory from given archetypes DOESN'T clean
         * entities nor their index effectively creating dangling entities;
         * \param archetype
         */
        void clean_archetype_component_arrays(archetype* archetype) {
            for (size_t j = 0; j < archetype->byte_arrays.size(); j++) {
                auto component
                    = component_map_->at(archetype->component_ids[j]).get();
                for (size_t i = 0; i < archetype->entities.size(); i++) {
                    component->destroy_data(
                        &archetype->byte_arrays[j]
                             .componentData[i * component->get_size()]);
                }
                delete[] archetype->byte_arrays[j].componentData;
                archetype->byte_arrays[j].size = 0;
            }
        }

        void try_move_last_entity_to(archetype* archetype,
                                     size_t destination_index) {
            const auto last_entity_index = archetype->entities.size() - 1;
            if (last_entity_index != destination_index) {
                const entity_t last_entity
                    = archetype->entities.at(last_entity_index);
                archetype->entities.at(destination_index) = last_entity;
                entity_index_->at(last_entity).index = destination_index;
            }
        }

        void move_entity_to_archetype(archetype* new_archetype,
                                      entity_t entity) {
            entity_index_->at(entity).entity_archetype = new_archetype;
            entity_index_->at(entity).index = new_archetype->entities.size();
            new_archetype->entities.push_back(entity);
        }

        template <typename C, typename... Args>
        void move_old_comp_and_emplace_new(archetype* old_archetype,
                                           archetype* new_archetype,
                                           size_t old_index, Args&&... args) {
            int old_component_index = 0;
            for (int i = 0; i < new_archetype->byte_arrays.size(); i++) {
                component_id_t component_type_id
                    = new_archetype->component_ids.at(i);
                component_base* component
                    = component_map_->at(component_type_id).get();
                if (component_type_id == type_id_generator::get<C>()) {
                    emplace_component<C>(new_archetype, i,
                                         std::forward<Args>(args)...);
                    continue;
                }
                move_component(old_archetype, new_archetype, old_index,
                               new_archetype->entities.size(),
                               old_component_index, i, component);
                if ((old_archetype->entities.size() - 1) != old_index) {
                    move_last_component(old_archetype, old_index,
                                        old_component_index, component);
                }
                old_component_index++;
            }
        }

        template <typename C>
        void move_comp_to_and_omit(archetype* old_archetype,
                                   archetype* new_archetype, size_t old_index) {
            int new_component_index = 0;
            size_t omited_component_index{
                archetype->component_id_index.at(type_id_generator::get<C>())};

            for (int i = 0; i < old_archetype->byte_arrays.size(); i++) {
                component_base* component
                    = component_map_->at(old_archetype->component_ids.at(i))
                          .get();
                if (i != omited_component_index && new_archetype) {
                    move_component(old_archetype, new_archetype, old_index,
                                   new_archetype->entities.size() - 1, i,
                                   new_component_index, component);

                    new_component_index++;
                } else {
                    erase_component(old_archetype, old_index, i, component);
                }
                if ((old_archetype->entities.size() - 1) != old_index) {
                    move_last_component(old_archetype, old_index, i, component);
                }
            }
        }

        /**
         * @brief Get the components of given entity in given archetype
         *
         * @tparam Cs components
         * @param archetype ptr to the archetype to retrieve component out of
         * @param entity_index index of the entity that components are requested
         * @return std::tuple<Cs&...> tuple of component reference
         */
        template <typename... Cs>
        std::tuple<Cs&...> get_components(archetype* archetype,
                                          size_t entity_index) {
            return std::tuple<Cs&...>(
                (get_component<Cs>(archetype, entity_index))...);
        }

        /**
         * @brief Get the component of given entity in given archetype
         *
         * @tparam C component
         * @param archetype ptr to the archetype to retrieve component out of
         * @param entity_index index of the entity that components are requested
         * @return C& reference to entity component
         */
        template <typename C>
        C& get_component(archetype* archetype, size_t index) {
            size_t component_index{
                archetype->component_id_index.at(type_id_generator::get<C>())};

            return *std::launder(
                reinterpret_cast<C*>(&archetype->byte_arrays[component_index]
                                          .componentData[index * sizeof(C)]));
        }

        template <typename F, typename Entity_T, typename... Cs>
        requires(std::invocable<F, entity_t, Cs...>) void apply(
            F&& f, type_list<Entity_T, Cs...> args_type, archetype* archetype) {
            auto tup = std::make_tuple(
                get_component_array<std::remove_reference_t<Cs>>(archetype)...);

            for (size_t i = 0; i < archetype->entities.size(); ++i) {
                f(archetype->entities.at(i),
                  std::get<decltype(get_component_array<std::remove_reference_t<Cs>>(archetype))>(tup)[i]...
                   );
            }
        }

       private:
        archetype_allocator archetype_allocator_;
        entity_index* entity_index_;
        component_map* component_map_;
    };

}  // namespace ant
