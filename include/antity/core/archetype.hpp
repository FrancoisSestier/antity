#pragma once
#include <antity/core/identifier.hpp>
#include <antity/utility/function_traits.hpp>
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

        [[nodiscard]] inline bool operator==(const archetype_key& other) const {
            if (this->signature != other.signature
                || this->chunk_id != other.chunk_id) {
                return false;
            }
            return true;
        }

        [[nodiscard]] inline bool operator!=(const archetype_key& other) const {
            return !(*this == other);
        }

        [[nodiscard]] inline bool match(const archetype_key& include) const {
            return (this->chunk_id == include.chunk_id
                    && (include.signature & ~this->signature).none());
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
        std::unordered_map<component_id_t, size_t> component_id_index;
        std::vector<byte_array> byte_arrays;
        std::vector<entity_t> entities;
    };

    template <typename... Cs>
    inline auto get_signature() {
        return signature_t{((get_type_signature<Cs>()) |= ...)};
    }

    template <typename Entity_T, typename... Cs>
    requires(std::is_same_v<Entity_T, entity_t>) inline auto get_signature(
        type_list<Entity_T, Cs...> types) {
        return get_signature<Cs...>();
    }

}  // namespace ant