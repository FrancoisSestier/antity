#pragma once

namespace ant
{

    typedef size_t IDType;
    typedef IDType ComponentTypeID;
    typedef IDType Entity;
    typedef IDType ChunkID;
    const IDType NULL_ENTITY = 0;

    template<typename T>
    class TypeIdGenerator {
        static IDType identifier() noexcept {
            static IDType value = 0;
            return value++;
        }

    public:
        template<typename U>
        static IDType GetNewID() noexcept {
            static const size_t value = identifier();
            return value;
        }
    };

}