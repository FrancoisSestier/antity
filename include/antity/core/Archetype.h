#pragma once
#include <cstddef>

#include "../utility/Hasher.h"
#include "Identifier.h"

namespace ant {
    struct Archetype;

    typedef std::byte* ComponentData;

    struct ByteArray {
        std::byte* componentData;
        size_t size;
    };

    struct ArchetypeKey {
        ArchetypeID archetypeId;
        ChunkID chunkId;

        struct hasher {
            size_t operator()(const ArchetypeKey& key) const {
                return hash::hash_combine(hash::hash(key.archetypeId),
                                          hash::hash(key.chunkId));
            }
        };

        struct comparator {
            bool operator()(const ArchetypeKey& lhs,
                            const ArchetypeKey& rhs) const {
                if (lhs.archetypeId.size() != rhs.archetypeId.size()
                    || lhs.chunkId != rhs.chunkId) {
                    return false;
                }

                for (size_t i = 0; i < lhs.archetypeId.size(); i++) {
                    if (lhs.archetypeId[i] != rhs.archetypeId[i]) {
                        return false;
                    }
                }

                return true;
            }
        };
    };

    struct Archetype {
        ArchetypeID archetypeId;
        Signature archetypeSignature;
        ChunkID chunkId;
        std::vector<ByteArray> byteArrays;
        std::vector<Entity> entities;
    };

    inline auto BuildArchetypeSignature(const Archetype* archetype) {
        Signature signature;
        for (auto componentId : archetype->archetypeId) {
            signature |= GetTypeSignature(componentId);
        }
        return signature;
    }

    inline auto BuildArchetypeSignature(const std::initializer_list<Signature>& archetypeID) {
        Signature signature;
        for (auto componentId : archetypeID) {
            signature |= componentId;
        }
        return signature;
    }

    template <typename... Cs>
    inline auto GetArchetypeSignature(){
        return Signature{((GetTypeSignature<Cs>()) |= ...)};
    }

}  // namespace ant