#pragma once
#include <cstdint>

typedef size_t IDType;
typedef IDType EntityID;
typedef IDType ComponentTypeID;
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
        static const std::size_t value = identifier();
        return value;
    }
};
