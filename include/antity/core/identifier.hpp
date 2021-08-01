#pragma once
#include <bitset>
#include <cstdint>
#include <vector>

namespace ant {

    using id_t = uint32_t;
    using component_id_t = id_t;
    using index_t = id_t;
    using entity_t = id_t;
    using chunk_id_t = id_t;
    inline constexpr id_t _null_entity = 0;
    inline constexpr id_t _null_chunk = UINT32_MAX;
    inline constexpr uint16_t _max_components = 64;
    using signature = std::bitset<_max_components>;
    using archetype_id = std::vector<component_id_t>;

    class type_id_generator {
        inline static id_t identifier() noexcept {
            static id_t value = 0;
            return value++;
        }

        template <typename T>
        inline static id_t get_sanitized_type_id() {
            static const id_t value = identifier();
            return value;
        }

       public:
        template <typename T>
        inline static id_t get() noexcept {
            return get_sanitized_type_id<
                std::remove_cv_t<std::remove_reference_t<T>>>();
        }
    };

    inline static signature get_type_signature(id_t Id) noexcept {
        return signature(1ULL << static_cast<uint64_t>(Id));
    }

    template <typename T>
    inline static signature get_type_signature() noexcept {
        return get_type_signature(type_id_generator::get<T>());
    }
}  // namespace ant
