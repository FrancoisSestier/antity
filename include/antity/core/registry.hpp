#pragma once
#include <antity/core/archetype_handler.hpp>
#include <antity/core/archetype_map.hpp>
#include <antity/core/component.hpp>
#include <antity/core/identifier.hpp>
#include <antity/core/record.hpp>
#include <antity/core/registry_debugger.hpp>
#include <antity/core/view.hpp>
#include <antity/utility/unique_id_dispenser.hpp>
#include <chrono>
#include <stdexcept>
#include <string>

namespace ant {

    class registry {
       public:
        using entity_type = entity_t;

        registry()
            : entity_index_(std::make_unique<entity_index>()),
              component_map_(std::make_unique<component_map>()),
              archetype_handler_(
                  archetype_handler{entity_index_.get(), component_map_.get()}),
              archetype_map_(&archetype_handler_) {}

        ~registry() {
            for (auto&& it : *archetype_map_.get()) {
                archetype_handler_.clean_archetype_component_arrays(
                    it.second.get());
            }
        }

        /**
         * \brief Add entity_t to the internal indexer
         * \return created entityID
         */
        entity_t create(chunk_id_t chunk_id = _null_chunk) {
            entity_t entity_t = entity_id_dispenser.get();
            entity_index_->emplace(entity_t, record_t{nullptr, 0, chunk_id});
            return entity_t;
        }

        /* Not Implemented !*/
        template <typename... Cs>
        entity_t create(chunk_id_t chunk_id, Cs&&...);

        /* Not Implemented  !*/
        void remove(entity_t entity_t) {
            // TODO REMOVE COMPONENTS
            entity_id_dispenser.free(entity_t);
        }

        /**
         * \brief Add Component To given entity_t
         * \tparam C ComponentType
         * \tparam Args Component Param Type
         * \param entity_t
         * \param chunk_id if no chunkID is Given current entity_t chunkID will
         * be used \param args Component arguments
         */
        template <typename C, typename... Args>
        void add(entity_t entity_t, Args&&... args);

        /**
         * @brief remove entity component
         *
         * @tparam C component to be removed
         * @param entity_t entity of wich the component needs to be removed
         */
        template <typename C>
        void remove(entity_t entity);

        template <typename... Cs>
        void save();

        /**
         * \brief builds a multiviews from given components in chunkID
         * \tparam Cs Component types to be retrieved
         * \param chunkid if no chunkID all chunkIDs will be in the multiview
         * \return a Multiview<entity_t,Cs...>
         */
        template <typename... Cs>
        auto get(chunk_id_t chunkid = _null_chunk);

        /**
         * \brief Get all given Component from an entity_t
         * \tparam Cs ComponentTypes to retrieve
         * \param entity_t
         * \return tuple of component refereces ;
         */
        template <typename... Cs>
        std::tuple<Cs&...> get_entity_components(entity_t entity_t);

       private:
        template <typename C>
        void save_impl();

        template <typename C, typename... Args>
        void add_impl(archetype* old_archetype, entity_t entity,
                      record_t record, Args&&... args);

        template <typename C>
        void remove_impl(archetype* old_archetype, entity_t entity,
                         record_t record);

        std::unique_ptr<entity_index> entity_index_;
        std::unique_ptr<component_map> component_map_;
        archetype_map archetype_map_;
        unique_id_dispenser<entity_t> entity_id_dispenser;
        archetype_handler archetype_handler_;
        registry_debugger registry_debugger_;
    };
    
    template <typename... Cs>
    entity_t registry::create(chunk_id_t chunk_id, Cs&&... cs) {
        entity_t entity = create(chunk_id);
        signature_t signature;
        ((signature |= get_type_signature<Cs>()), ...);
        archetype* new_archetype
            = archetype_map_.get(archetype_key{signature, chunk_id});
        ((archetype_handler_.insert_component<Cs>(new_archetype,
                                                 std::forward<Cs>(cs))),
         ...);
        entity_index_->at(entity).entity_archetype = new_archetype;
        entity_index_->at(entity).index = new_archetype->entities.size();
        new_archetype->entities.push_back(entity);
        return entity;
    }

    template <typename C, typename... Args>
    void registry::add(entity_t entity, Args&&... args) {
        if (!entity_index_->contains(entity)) {
            throw std::runtime_error("unregisterd entity_t");
        }
        if (!component_map_->contains(type_id_generator::get<C>())) {
            save<C>();
        }
        add_impl<C>(entity_index_->at(entity).entity_archetype, entity,
                    entity_index_->at(entity), std::forward<Args...>(args...));
    }

    template <typename C>
    void registry::remove(entity_t entity) {
        if (!entity_index_->contains(entity)) {
            throw std::runtime_error("unregisterd entity_t");
        }
        if (!component_map_->contains(type_id_generator::get<C>())) {
            save<C>();
        }
        remove_impl<C>(entity_index_->at(entity).entity_archetype, entity,
                       entity_index_->at(entity));
    }

    template <typename... Cs>
    void registry::save() {
        (save_impl<Cs>(), ...);
    }

    template <typename C>
    void registry::save_impl() {
        static_assert(alignof(C) <= sizeof(C),
                      "Align requirement of a component can not be superior to "
                      "it's size");

        component_id_t component_type_id
            = static_cast<component_id_t>(type_id_generator::get<C>());
        if (component_map_->contains(component_type_id)) {
            throw std::runtime_error("Trying To Register Component Twice");
        }
        component_map_->emplace(component_type_id,
                                std::make_unique<Component<C>>());
        registry_debugger_.on_component_registration<C>();
        archetype_map_.on_component_registration(type_id_generator::get<C>());
    }

    template <typename C, typename... Args>
    void registry::add_impl(archetype* old_archetype, entity_t entity,
                            record_t record, Args&&... args) {
        archetype* new_archetype{nullptr};
        if (old_archetype == nullptr) {
            new_archetype = archetype_map_.get(
                archetype_key{get_type_signature<C>(), record.chunk_id});
            archetype_handler_.emplace_component<C>(
                new_archetype, 0, std::forward<Args>(args)...);
        } else {
            new_archetype
                = archetype_map_.get_next_archetype_add<C>(old_archetype);

            archetype_handler_.move_old_comp_and_emplace_new<C>(
                old_archetype, new_archetype, record.index,
                std::forward<Args...>(args...));

            archetype_handler_.try_move_last_entity_to(old_archetype,
                                                       record.index);
            old_archetype->entities.pop_back();

            if (old_archetype->entities.empty()) {
                archetype_map_.delete_archetype(old_archetype->key);
            }
        }
        entity_index_->at(entity).entity_archetype = new_archetype;
        entity_index_->at(entity).index = new_archetype->entities.size();
        new_archetype->entities.push_back(entity);
    }

    template <typename C>
    void registry::remove_impl(archetype* old_archetype, entity_t entity,
                               record_t record) {
        archetype* new_archetype = archetype* new_archetype
            = archetype_map_.get_next_archetype_remove<C>(old_archetype);

        archetype_handler_.move_comp_to_and_omit<C>(
            old_archetype, new_archetype, record.index);

        archetype_handler_.try_move_last_entity_to(old_archetype, record.index);

        old_archetype->entities.pop_back();

        if (old_archetype->entities.empty()) {
            archetype_map_.delete_archetype(old_archetype->key);
        }
        if (new_archetype) {
            archetype_handler_.move_entity_to_archetype(new_archetype, entity);
        }
    }

    template <typename... Cs>
    inline auto registry::get(chunk_id_t chunk_id) {
        return archetype_map_view<Cs...>{&archetype_map_,
                                         get_signature<Cs...>(), chunk_id};
    }

    template <typename... Cs>
    std::tuple<Cs&...> registry::get_entity_components(entity_t entity) {
        return archetype_handler_.get_components<Cs...>(
            entity_index_->at(entity).entity_archetype,
            entity_index_->at(entity).index);
    }
}  // namespace ant
