#pragma once
#include <antity/core/identifier.hpp>
#include <antity/utility/hasher.hpp>
#include <cstddef>

namespace ant {
    struct archetype;

    typedef std::byte* ComponentData;

    struct byte_array {
        std::byte* componentData;
        size_t size;
    };

    struct archetype_key {
        archetype_id archetype_id;
        chunk_id_t chunk_id;

        struct hasher {
            size_t operator()(const archetype_key& key) const {
                return hash::hash_combine(hash::hash(key.archetype_id),
                                          hash::hash(key.chunk_id));
            }
        };

        struct comparator {
            bool operator()(const archetype_key& lhs,
                            const archetype_key& rhs) const {
                if (lhs.archetype_id.size() != rhs.archetype_id.size()
                    || lhs.chunk_id != rhs.chunk_id) {
                    return false;
                }

                for (size_t i = 0; i < lhs.archetype_id.size(); i++) {
                    if (lhs.archetype_id[i] != rhs.archetype_id[i]) {
                        return false;
                    }
                }

                return true;
            }
        };
    };

    struct archetype {
        archetype_id archetype_id;
        signature archetypesignature;
        chunk_id_t chunk_id;
        std::vector<byte_array> byteArrays;
        std::vector<entity_t> entities;
    };

    inline auto build_archetype_signature(const archetype* archetype) {
        signature signature;
        for (auto componentId : archetype->archetype_id) {
            signature |= get_type_signature(componentId);
        }
        return signature;
    }

    inline auto build_archetype_signature(
        const std::initializer_list<signature>& archetype_id) {
        signature signature;
        for (auto componentId : archetype_id) {
            signature |= componentId;
        }
        return signature;
    }

    template <typename... Cs>
    inline auto get_signature() {
        return signature{((get_type_signature<Cs>()) |= ...)};
    }

}  // namespace ant