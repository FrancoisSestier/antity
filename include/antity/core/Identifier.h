#pragma once
#include <cstdint>
#include <vector>

namespace ant
{

    typedef uint32_t IDType;
    typedef IDType ComponentTypeID;
    typedef IDType Entity;
    typedef IDType ChunkID;
    const IDType NULL_ENTITY = 0;
    constexpr IDType NULL_CHUNK = UINT32_MAX;
    typedef std::vector<ComponentTypeID> ArchetypeID;
	
    class TypeIdGenerator {
         static IDType identifier() noexcept {
                static IDType value = 0;
                return value++;
         }

        public:
            template<typename> static IDType GetTypeID() noexcept {
                static const IDType value = identifier();
                return value;
            }
    };

}
