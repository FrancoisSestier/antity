#pragma once
#include <algorithm>
#include <antity/core/archetype.hpp>
#include <antity/utility/robin_hood.hpp>
#include <memory>
#include <unordered_map>

namespace ant {
    class archetype_map {
       public:
        using archetype_hashtable
            = std::unordered_map<archetype_key, std::unique_ptr<archetype>,
                                 archetype_key::hasher,
                                 archetype_key::comparator>;

        archetype_map(archetype_handler* handler)
            : archetype_handler_(handler) {}

        archetype_hashtable* get() { return &archetype_hashtable_; }

        /**
         * \brief get that match exactly the archetype_key
         *		  if no archetype of such type exists creates it
         * \param archetype_key requested archetype key
         * \return archetype* requested archetype
         */
        archetype* get(const archetype_key& archetype_key) {
            if (!archetype_hashtable_.contains(archetype_key)) {
                create_archetype(archetype_key);
            }
            return archetype_hashtable_.at(archetype_key).get();
        }

        /**
         * @brief get and iterator to the first element of underlying
         *        vector of signatures matching at list all components
         *        of given signature_t
         *
         * @param match signature containing all required components
         * @return auto iterator to fisrt matching element of the underlying
         *         vector of signatures
         */
        auto signatures_begin(const archetype_key& match) {
            return std::find_if(archetype_keys_.begin(),
                                archetype_keys_.end(),
                                [&](const archetype_key& key) {
                                    return (match.signature & ~key.signature).none() && (match.chunk_id == key.chunk_id) ;
                                });
        }

        /**
         * @brief get next archetype obtained by adding given component to
         * current archetype
         *
         * @tparam C component to add to the current archetype
         * @param current ptr to the current archetype
         * @return auto archetype containing all components of current archetype
         * plus C
         */
        template <typename C>
        archetype* get_next_archetype_add(archetype* current) {
            signature_t new_archetype_signature
                = add_type_to_signature<C>(current->key.signature);
            return get({new_archetype_signature,current->key.chunk_id});
        }

        template <typename C>
        archetype* get_next_archetype_remove(archetype* current) {
            signature_t new_archetype_signature
                = remove_type_to_signature<C>(current->key.signature);
            if (new_archetype_signature.none()) {
                return nullptr;
            }
            return get({new_archetype_signature,current->key.chunk_id});
        }

        /**
         * @brief get and iterator to the last element of underlying
         *        vector of signatures
         *
         * @return auto iterator to the last element of underlying
         *        vector of signatures
         */
        auto signatures_end() { return archetype_keys_.end(); }

        /**
         * @brief deletes archetype associated with given key
         *        effectively cleaning it's component_arrays
         *
         * @param key archetype to be deleted
         */
        void delete_archetype(const archetype_key& key) {
            archetype_handler_->clean_archetype_component_arrays(
                archetype_hashtable_.at(key).get());
            archetype_hashtable_.erase(key);
            std::erase(archetype_keys_,key);
        }

        void on_component_registration(component_id_t component_type_id) {}

       private:
        void create_archetype(const archetype_key& key) {
            const component_id_list component_ids
                = signature_to_type_ids(key.signature);
            auto new_archetype
                = std::make_unique<archetype>(key, component_ids);

            for (auto&& componentID : component_ids) {
                new_archetype->byte_arrays.push_back(
                    byte_array{new std::byte[0], 0});
            }

            archetype_keys_.push_back(key);

            archetype_hashtable_.emplace(key, std::move(new_archetype));
        }

       private:
        archetype_handler* archetype_handler_;
        archetype_hashtable archetype_hashtable_;
        std::vector<archetype_key> archetype_keys_;
    };
}  // namespace ant
