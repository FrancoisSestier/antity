#pragma once
#include <bitset>
#include <cstdint>
#include <vector>

namespace ant {

    using IDType = uint32_t;
    using ComponentTypeID = IDType;
    using Entity = IDType;
    using ChunkID = IDType;
    inline constexpr IDType NULL_ENTITY = 0;
    inline constexpr IDType NULL_CHUNK = UINT32_MAX;
    inline constexpr uint16_t MAX_COMPONENTS = 64;
    using Signature = std::bitset<MAX_COMPONENTS>;
    using ArchetypeID = std::vector<ComponentTypeID>;

    class TypeIdGenerator {
        inline static IDType identifier() noexcept {
            static IDType value = 0;
            return value++;
        }

        template <typename T>
        inline static IDType GetSanitizedTypeID() {
            static const IDType value = identifier();
            return value;
        }

       public:
        template <typename T>
        inline static IDType GetTypeID() noexcept {
            return GetSanitizedTypeID<
                std::remove_cv_t<std::remove_reference_t<T>>>();
        }
    };

    inline Signature GetTypeSignature(IDType Id) noexcept {
        return Signature(1ULL << static_cast<uint64_t>(Id));
    }

    template <typename T>
    inline Signature GetTypeSignature() noexcept {
        return GetTypeSignature(TypeIdGenerator::GetTypeID<T>());
    }
}  // namespace ant
