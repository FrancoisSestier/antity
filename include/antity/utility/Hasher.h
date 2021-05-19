#pragma once
#include <limits>
#include <cstdint>
#include <concepts>
#include <vector>

namespace ant {
    namespace  hash {
        template<typename T>
        static T xorshift(const T& n, int i) {
            return n ^ (n >> i);
        }

        inline static uint32_t distribute(const uint32_t& n) {
            uint32_t p = 0x55555555ul; // pattern of alternating 0 and 1
            uint32_t c = 3423571495ul; // random uneven integer constant; 
            return c * xorshift(p * xorshift(n, 16), 16);
        }

        template<std::integral T>
        static uint64_t hash(const  T& n) {
            uint64_t p = 0x5555555555555555;     // pattern of alternating 0 and 1
            uint64_t c = 17316035218449499591ull;// random uneven integer constant; 
            return c * xorshift(p * xorshift(n, 32), 32);
        }
    	
        template <class T>
        inline static size_t hash_combine(const std::size_t& seed, const T& v)
        {
            return std::rotl(seed, std::numeric_limits<size_t>::digits / 3) ^ distribute(static_cast<uint32_t>(hash<T>(v)));
        }
    	
        template<std::integral T>
        static size_t hash(const std::vector<T>& v) {
            std::size_t h = hash(0);
            for (const auto i : v)
            {
                h = hash_combine(h, i);
            }
            return h;
        }

    }
}