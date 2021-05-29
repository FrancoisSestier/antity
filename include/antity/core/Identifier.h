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
         inline static IDType identifier() noexcept {
                static IDType value = 0;
                return value++;
         }
         
         template<typename T> 
         inline static IDType GetSanitizedTypeID() {
            static const IDType value = identifier();
            return value;
         }

        public:
            template<typename T> 
            inline static IDType GetTypeID() noexcept {
                return GetSanitizedTypeID<std::remove_cv_t<std::remove_reference_t<T>>>();
            }
    };

}
