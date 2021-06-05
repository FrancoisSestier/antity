#pragma once
#include "Archetype.h"
#include "Identifier.h"
#include "antity/utility/robin_hood.h"

namespace ant {

    struct Record {
        Archetype* archetype = nullptr;
        IDType index = 0;
        ChunkID chunkId = 0;
    };

    using EntityIndex = robin_hood::unordered_map<Entity, Record>;

}  // namespace ant