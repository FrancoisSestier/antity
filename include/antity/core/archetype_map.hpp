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
         * @brief Get the archetype for given signature_t
         *
         * @param signature_t requested archetype signature_t
         * @return archetype* requested archetype
         */
        archetype* get(const signature_t& signature_t) {
            return signature_archetype_map_.at(signature_t);
        }

        auto signaturesBegin(const signature_t& match) {
            return std::find_if(archetype_signatures_.begin(),
                                archetype_signatures_.end(),
                                [&](const signature_t& signature_t) {
                                    return (match & ~signature_t).none();
                                });
        }

        auto signaturesEnd() { return archetype_signatures_.end(); }

        /**
         * @brief deletes archetype associated with given key
         *
         * @param archetype_key archetype to be deleted
         */
        void Deletearchetype(const archetype_key& archetype_key) {
            deregister_archetype_signature(
                archetype_hashtable_.at(archetype_key).get());
            archetype_hashtable_.erase(archetype_key);
        }

        void OnComponentRegistration(component_id_t componentTypeId) {}

        /**
         * \brief Retrieve all archetypes that have at least all components in
         * givent ArchetpeKey \param archetype_id_t \param chunk_id \return
         * vector<archetype*>
         */
        std::vector<archetype*> gets(archetype_id_t archetype_id_t,
                                     chunk_id_t chunk_id = _null_chunk) {
            std::vector<archetype*> archetypes;
            for (auto&& archetype : archetype_hashtable_) {
                if (!std::ranges::includes(
                        archetype.second->archetype_id.begin(),
                        archetype.second->archetype_id.end(),
                        archetype_id_t.begin(), archetype_id_t.end())) {
                    continue;
                }
                if (chunk_id != _null_chunk
                    && archetype.second->chunk_id != chunk_id) {
                    continue;
                }
                archetypes.emplace_back(archetype.second.get());
            }

            return std::move(archetypes);
        }

       private:
        void create_archetype(const archetype_key& archetype_key) {
            auto newarchetype = std::make_unique<archetype>(
                archetype_key.archetype_id, archetype_key.chunk_id);

            archetype_hashtable_.emplace(archetype_key,
                                         std::move(newarchetype));
            for (auto&& componentID : archetype_key.archetype_id) {
                archetype_hashtable_.at(archetype_key)
                    ->byte_arrays.push_back(byte_array{new std::byte[0], 0});
            }
            register_archetype_signature(
                archetype_hashtable_.at(archetype_key).get());
        }

        inline void register_archetype_signature(archetype* archetype) {
            archetype->signature
                = build_archetype_signature(archetype);
            archetype_signatures_.emplace_back(archetype->signature);
            signature_archetype_map_.emplace(archetype->signature,
                                             archetype);
        }

        inline void deregister_archetype_signature(archetype* arch) {
            archetype_signatures_.erase(std::find(
                archetype_signatures_.begin(), archetype_signatures_.end(),
                arch->signature));
            signature_archetype_map_.erase(arch->signature);
        }

       private:
        archetype_hashtable archetype_hashtable_;
        std::vector<signature_t> archetype_signatures_;
        std::unordered_map<signature_t, archetype*> signature_archetype_map_;
    };
}  // namespace ant
