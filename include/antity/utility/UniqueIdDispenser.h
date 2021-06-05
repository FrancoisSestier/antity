#pragma once
#include <concepts>
#include <queue>

namespace ant {

    /// <summary>
    /// UniqueID Dispenser
    /// </summary>
    /// <typeparam name="T">must be unsigned</typeparam>
    template <std::unsigned_integral T>
    class UniqueIdDispenser {
       public:
        UniqueIdDispenser() = default;

        /// <summary>
        /// Ctor you can exclude some IDs by setting start id to a value > 0
        /// All IDs before start will never be used / given on set id exept if
        /// you explicitly free it using FreeID
        /// </summary>
        /// <returns></returns>
        UniqueIdDispenser(T start) { idCount = start; }

        /// <summary>
        /// Get a Unique ID
        /// </summary>
        /// <returns> unsigned ID </returns>
        T GetID() {
            if (recycled.empty()) {
                return ++idCount;
            }
            auto id = recycled.front();
            recycled.pop();
            return id;
        }

        /// <summary>
        /// Free unused ID
        /// </summary>
        void FreeID(T id) { recycled.push(id); }

       private:
        std::queue<T> recycled;
        T idCount = 0;
    };

}  // namespace ant
