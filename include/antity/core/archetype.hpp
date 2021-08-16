#pragma once
#include <antity/core/identifier.hpp>
#include <antity/utility/hasher.hpp>
#include <cstddef>
#include <functional>

namespace ant {
    struct archetype;

    typedef std::byte* ComponentData;

    struct byte_array {
        std::byte* componentData;
        size_t size;
    };

    struct archetype_key {
        signature_t signature;
        chunk_id_t chunk_id;

        inline bool operator==(const archetype_key& other) const {
            if (this->signature != other.signature
                || this->chunk_id != other.chunk_id) {
                return false;
            }
            return true;
        }

        inline bool operator!=(const archetype_key& other) const {
            return !(*this == other);
        }

        struct hasher {
            size_t operator()(const archetype_key& key) const {
                return hash::hash_combine(
                    std::hash<signature_t>{}(key.signature),
                    hash::hash(key.chunk_id));
            }
        };

        struct comparator {
            bool operator()(const archetype_key& lhs,
                            const archetype_key& rhs) const {
                return lhs == rhs;
            }
        };
    };

    struct archetype {
        const archetype_key key;
        const component_id_list component_ids;
        std::vector<byte_array> byte_arrays;
        std::vector<entity_t> entities;
    };

    template <typename... Cs>
    inline auto get_signature() {
        return signature_t{((get_type_signature<Cs>()) |= ...)};
    }

}  // namespace ant