#pragma once
#include <algorithm>
#include <memory>
#include <unordered_map>

#include "Archetype.h"
#include "antity/utility/robin_hood.h"

namespace ant {
    class ArchetypeMap {
       public:
        using ArchetypeHashTable
            = std::unordered_map<ArchetypeKey, std::unique_ptr<Archetype>,
                                 ArchetypeKey::hasher,
                                 ArchetypeKey::comparator>;

        ArchetypeHashTable* get() { return &archetypeHashTable; }

        /**
         * \brief GetArchetype that match exactly the archetypeKey
         *		  if no archetype of such type exists creates it
         * \param archetypeKey requested archetype key
         * \return Archetype* requested archetype
         */
        Archetype* GetArchetype(const ArchetypeKey& archetypeKey) {
            if (!archetypeHashTable.contains(archetypeKey)) {
                CreateArchetype(archetypeKey);
            }
            return archetypeHashTable.at(archetypeKey).get();
        }

        /**
         * @brief Get the Archetype for given signature
         *
         * @param signature requested archetype signature
         * @return Archetype* requested archetype
         */
        Archetype* GetArchetype(const Signature& signature) {
            return signatureArchetypeMap.at(signature);
        }

        auto signaturesBegin(const Signature& match) {
            return std::find_if(archetypeSignatures.begin(),
                                archetypeSignatures.end(),
                                [&](const Signature& signature) {
                                    return (match & ~signature).none();
                                });
        }

        auto signaturesEnd() { return archetypeSignatures.end(); }

        /**
         * @brief deletes archetype associated with given key
         *
         * @param archetypeKey archetype to be deleted
         */
        void DeleteArchetype(const ArchetypeKey& archetypeKey) {
            DeRegisterArchetypeSignature(
                archetypeHashTable.at(archetypeKey).get());
            archetypeHashTable.erase(archetypeKey);
        }

        void OnComponentRegistration(ComponentTypeID componentTypeId) {}

        /**
         * \brief Retrieve all archetypes that have at least all components in
         * givent ArchetpeKey \param archetypeId \param chunkId \return
         * vector<Archetype*>
         */
        std::vector<Archetype*> GetArchetypes(ArchetypeID archetypeId,
                                              ChunkID chunkId = NULL_CHUNK) {
            std::vector<Archetype*> archetypes;
            for (auto&& archetype : archetypeHashTable) {
                if (!std::ranges::includes(
                        archetype.second->archetypeId.begin(),
                        archetype.second->archetypeId.end(),
                        archetypeId.begin(), archetypeId.end())) {
                    continue;
                }
                if (chunkId != NULL_CHUNK
                    && archetype.second->chunkId != chunkId) {
                    continue;
                }
                archetypes.emplace_back(archetype.second.get());
            }

            return std::move(archetypes);
        }

       private:
        void CreateArchetype(const ArchetypeKey& archetypeKey) {
            auto newArchetype = std::make_unique<Archetype>(
                archetypeKey.archetypeId, archetypeKey.chunkId);

            archetypeHashTable.emplace(archetypeKey, std::move(newArchetype));
            for (auto&& componentID : archetypeKey.archetypeId) {
                archetypeHashTable.at(archetypeKey)
                    ->byteArrays.push_back(ByteArray{new std::byte[0], 0});
            }
            RegisterArchetypeSignature(
                archetypeHashTable.at(archetypeKey).get());
        }

        inline void RegisterArchetypeSignature(Archetype* archetype) {
            archetype->archetypeSignature = BuildArchetypeSignature(archetype);
            archetypeSignatures.emplace_back(archetype->archetypeSignature);
            signatureArchetypeMap.emplace(archetype->archetypeSignature,
                                          archetype);
        }

        inline void DeRegisterArchetypeSignature(Archetype* archetype) {
            archetypeSignatures.erase(std::find(archetypeSignatures.begin(),
                                                archetypeSignatures.end(),
                                                archetype->archetypeSignature));
            signatureArchetypeMap.erase(archetype->archetypeSignature);
        }

       private:
        ArchetypeHashTable archetypeHashTable;
        std::vector<Signature> archetypeSignatures;
        std::unordered_map<Signature, Archetype*> signatureArchetypeMap;
    };
}  // namespace ant
