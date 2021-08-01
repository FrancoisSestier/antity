#pragma once
#include <concepts>
#include <queue>

namespace ant {

    /// <summary>
    /// UniqueID Dispenser
    /// </summary>
    /// <typeparam name="T">must be unsigned</typeparam>
    template <std::unsigned_integral T>
    class unique_id_dispenser {
       public:
        unique_id_dispenser() = default;

        /// <summary>
        /// Ctor you can exclude some IDs by setting start id to a value > 0
        /// All IDs before start will never be used / given on set id exept if
        /// you explicitly free it using free
        /// </summary>
        /// <returns></returns>
        unique_id_dispenser(T start) { id_count_ = start; }

        /// <summary>
        /// Get a Unique ID
        /// </summary>
        /// <returns> unsigned ID </returns>
        T get() {
            if (recycled_.empty()) {
                return ++id_count_;
            }
            auto id = recycled_.front();
            recycled_.pop();
            return id;
        }

        /// <summary>
        /// Free unused ID
        /// </summary>
        void free(T id) { recycled_.push(id); }

       private:
        std::queue<T> recycled_;
        T id_count_ = 0;
    };

}  // namespace ant
