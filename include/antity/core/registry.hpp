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
              archetype_handler_(archetype_handler{entity_index_.get(),
                                                   component_map_.get()}) {}

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
        entity_t create(chunk_id_t chunk_id = 0) {
            entity_t entity_t = entity_id_dispenser.get();
            entity_index_->emplace(entity_t, Record{nullptr, 0, chunk_id});
            return entity_t;
        }

        /* Not Implemented !*/
        template <typename... Cs>
        entity_t create(chunk_id_t chunk_id = 0, Cs&&...) {
            entity_t entity_t = entity_id_dispenser.get();
            entity_index_->emplace(entity_t, Record{nullptr, 0, chunk_id});
            /* TODO */
            return entity_t;
        }

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

        /* Not Implemented  !*/
        template <typename C>
        void remove(entity_t entity_t);

        template <typename... Cs>
        void save();

        template <typename C>
        void save_impl();

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
       private:
        std::unique_ptr<entity_index> entity_index_;
        std::unique_ptr<component_map> component_map_;
        archetype_map archetype_map_;
        unique_id_dispenser<entity_t> entity_id_dispenser;
        archetype_handler archetype_handler_;
        registry_debugger registry_debugger_;
    };

    template <typename C, typename... Args>
    void registry::add(entity_t e, Args&&... args) {
        if (!entity_index_->contains(e)) {
            throw std::runtime_error("unregisterd entity_t");
        }
        component_id_t newComponentTypeId = type_id_generator::get<C>();

        if (!component_map_->contains(newComponentTypeId)) {
            save<C>();
        }

        const Record record = entity_index_->at(e);

        archetype* oldarchetype = record.archetype;
        archetype* newarchetype;

        if (oldarchetype == nullptr) {
            newarchetype = archetype_map_.get(archetype_key{
                archetype_id{newComponentTypeId}, record.chunk_id});
            component_base* component
                = component_map_->at(newComponentTypeId).get();
            archetype_handler_.emplace_component<C>(
                newarchetype, component, 0, std::forward<Args>(args)...);
        } else {
            archetype_id oldarchetype_id = oldarchetype->archetype_id;
            archetype_id newarchetype_id = oldarchetype_id;
            newarchetype_id.push_back(newComponentTypeId);
            std::ranges::sort(newarchetype_id);
            newarchetype = archetype_map_.get(
                archetype_key{newarchetype_id, record.chunk_id});
            const auto oldIndex = record.index;
            const auto Lastentity_index = oldarchetype->entities.size() - 1;
            int oldCompIndex = 0;
            for (int i = 0; i < newarchetype->byteArrays.size(); i++) {
                component_id_t componentTypeId
                    = newarchetype->archetype_id.at(i);
                component_base* component
                    = component_map_->at(componentTypeId).get();
                if (componentTypeId == newComponentTypeId) {
                    archetype_handler_.emplace_component<C>(
                        newarchetype, component, i,
                        std::forward<Args>(args)...);
                    continue;
                }
                archetype_handler_.move_component(
                    oldarchetype, newarchetype, oldIndex,
                    newarchetype->entities.size(), oldCompIndex, i, component);
                if (Lastentity_index != oldIndex) {
                    archetype_handler_.move_last_component(
                        oldarchetype, oldIndex, oldCompIndex, component);
                }
                oldCompIndex++;
            }
            if (Lastentity_index != oldIndex) {
                const entity_t lastentity
                    = oldarchetype->entities.at(Lastentity_index);
                oldarchetype->entities.at(oldIndex) = lastentity;
                entity_index_->at(lastentity).index = oldIndex;
            }
            oldarchetype->entities.pop_back();
            if (oldarchetype->entities.empty()) {
                archetype_handler_.clean_archetype_component_arrays(
                    oldarchetype);
                archetype_map_.Deletearchetype(archetype_key{
                    oldarchetype->archetype_id, oldarchetype->chunk_id});
            }
        }

        entity_index_->at(e).archetype = newarchetype;
        entity_index_->at(e).index = newarchetype->entities.size();
        newarchetype->entities.push_back(e);
    }

    template <typename C>
    void registry::remove(entity_t e) {
        const Record record = entity_index->at(e);

        archetype* oldarchetype = record.archetype;
        archetype* newarchetype = nullptr;
        component_id_t componentToRemoveTypeId = type_id_generator::get<C>();
        archetype_id newarchetype_id = oldarchetype->archetype_id;
        auto componentToRemove
            = std::ranges::find(newarchetype_id.begin(), newarchetype_id.end(),
                                componentToRemoveTypeId);
        auto componentToRemoveIndex
            = componentToRemove - newarchetype_id.begin();
        newarchetype_id.erase(componentToRemove);
        const auto oldIndex = record.index;
        const auto Lastentity_index = oldarchetype->entities.size() - 1;
        if (newarchetype_id.size() != 0) {
            newarchetype = archetype_map.get(
                archetype_key{newarchetype_id, oldarchetype->chunk_id});
            entity_index_->at(e).archetype = newarchetype;
            entity_index_->at(e).index = newarchetype->entities.size();
            newarchetype->entities.push_back(e);
        }
        int c = 0;
        for (int i = 0; i < oldarchetype->byteArrays.size(); i++) {
            component_base* component
                = component_map_->at(oldarchetype->archetype_id.at(i)).get();
            if (i != componentToRemoveIndex && newarchetype_id.size() != 0) {
                if (newarchetype != nullptr) {
                    archetype_handler_.move_component(
                        oldarchetype, newarchetype, oldIndex,
                        newarchetype->entities.size() - 1, i, c, component);
                }
                c++;
            } else {
                archetype_handler_.erase_component(oldarchetype, oldIndex, i,
                                                   component);
            }
            if (Lastentity_index != oldIndex) {
                archetype_handler_.move_last_component(oldarchetype, oldIndex,
                                                       i, component);
            }
        }
        if (Lastentity_index != oldIndex) {
            const entity_t lastentity
                = oldarchetype->entities.at(Lastentity_index);
            oldarchetype->entities.at(oldIndex) = lastentity;
            entity_index_->at(lastentity).index = oldIndex;
        }
        oldarchetype->entities.pop_back();

        if (oldarchetype->entities.empty()) {
            archetype_handler_.clean_archetype_component_arrays(oldarchetype);
            archetype_map_.Deletearchetype(archetype_key{
                oldarchetype->archetype_id, oldarchetype->chunk_id});
        }
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

        component_id_t componentTypeId
            = static_cast<component_id_t>(type_id_generator::get<C>());
        if (component_map_->contains(componentTypeId)) {
            throw std::runtime_error("Trying To Register Component Twice");
        }
        component_map_->emplace(componentTypeId,
                                std::make_unique<Component<C>>());
        registry_debugger_.OnComponentRegistration<C>();
        archetype_map_.OnComponentRegistration(type_id_generator::get<C>());
    }

    template <typename... Cs>
    inline auto registry::get(chunk_id_t chunkID) {
        return archetype_mapView<Cs...>{&archetype_map_,
                                        get_signature<Cs...>()};
    }

    template <typename... Cs>
    std::tuple<Cs&...> registry::get_entity_components(entity_t e) {
        return archetype_handler_.get_components<Cs...>(
            entity_index_->at(e).archetype, entity_index_->at(e).index);
    }
}  // namespace ant
