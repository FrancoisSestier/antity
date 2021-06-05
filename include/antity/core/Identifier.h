#pragma once
#include <cstdint>
#include <vector>
#include <bitset>

namespace ant
{

    typedef uint32_t IDType;
    typedef IDType ComponentTypeID;
    typedef IDType Entity;
    typedef IDType ChunkID;
    inline constexpr IDType NULL_ENTITY = 0;
    inline constexpr IDType NULL_CHUNK = UINT32_MAX;
    inline constexpr uint16_t MAX_COMPONENTS = 64;
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

    inline static auto GetTypeSignature(IDType Id) noexcept {
        return std::bitset<MAX_COMPONENTS>(1ULL << static_cast<uint64_t>(Id));
    }

    template<typename T>
    inline static auto GetTypeSignature() noexcept {
        return GetTypeSignature(TypeIdGenerator::GetTypeID<T>());
    }
}
