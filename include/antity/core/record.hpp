#pragma once
#include <antity/core/archetype.hpp>
#include <antity/core/identifier.hpp>
#include <antity/utility/robin_hood.hpp>

namespace ant {

    struct record_t {
        archetype* entity_archetype = nullptr;
        index_t index = 0;
        chunk_id_t chunk_id = 0;
    };

    using entity_index = robin_hood::unordered_map<entity_t, record_t>;

}  // namespace ant