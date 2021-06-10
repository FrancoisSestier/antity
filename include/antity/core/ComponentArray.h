#include "Archetype.h"

namespace ant {

    template <typename C>
    struct ComponentArray {
        struct ComponentIterator {
            using iterator_category = std::random_access_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = C;
            using pointer = C*;    // or also value_type*
            using reference = C&;  // or also value_type&

            ComponentIterator() : m_ptr(nullptr) {}
            ComponentIterator(C* rhs) : m_ptr(rhs) {}
            ComponentIterator(const ComponentIterator& rhs) : m_ptr(rhs.m_ptr) {}

            pointer get() { return m_ptr; }

            reference operator*() const { return *m_ptr; }
            pointer operator->() { return m_ptr; }

            difference_type operator-(const ComponentIterator& rhs) {
                return m_ptr - rhs.m_ptr;
            }
            ComponentIterator operator+(const difference_type& rhs) {
                return ComponentIterator(m_ptr + rhs);
            }
            ComponentIterator operator-(const difference_type& rhs) {
                return ComponentIterator(m_ptr - rhs);
            }
            ComponentIterator& operator+=(const difference_type& rhs) {
                m_ptr += rhs;
                return *this;
            }
            ComponentIterator& operator-=(const difference_type& rhs) {
                m_ptr -= rhs;
                return *this;
            }

            reference operator[](const difference_type& rhs) {
                return m_ptr[rhs];
            }

            ComponentIterator& operator++() {
                m_ptr++;
                return *this;
            }
            ComponentIterator operator++(int) {
                ComponentIterator tmp = *this;
                ++(*this);
                return tmp;
            }
            ComponentIterator& operator--() {
                m_ptr--;
                return *this;
            }
            ComponentIterator operator--(int) {
                ComponentIterator tmp = *this;
                --(*this);
                return tmp;
            }

            bool operator>(const ComponentIterator& rhs) const {
                return m_ptr > rhs.m_ptr;
            }
            bool operator<(const ComponentIterator& rhs) const {
                return m_ptr < rhs.m_ptr;
            }
            bool operator>=(const ComponentIterator& rhs) const {
                return m_ptr >= rhs.m_ptr;
            }
            bool operator<=(const ComponentIterator& rhs) const {
                return m_ptr <= rhs.m_ptr;
            }

            friend bool operator==(const ComponentIterator& a, const ComponentIterator& b) {
                return a.m_ptr == b.m_ptr;
            };
            friend bool operator!=(const ComponentIterator& a, const ComponentIterator& b) {
                return a.m_ptr != b.m_ptr;
            };

           private:
            pointer m_ptr;
        };
        ComponentArray() = default;

        ComponentArray(ByteArray* byteArray)
            : data(reinterpret_cast<C*>(byteArray->componentData)),
              byteArray(byteArray) {}

        [[nodiscard]] const C& operator[](size_t index) const {
            return data[index];
        }

        [[nodiscard]] C& operator[](size_t index) { return data[index]; }

        bool valid() const noexcept { return data != nullptr; }

        C* begin() { return data; }

        C* end() { return data + byteArray->size; }

        int16_t size() { return byteArray->size; }

        C* data{nullptr};
        ByteArray* byteArray{nullptr};
    };

    template <typename T>
    auto GetComponentArray(Archetype* archetype) {
        auto typeId = TypeIdGenerator::GetTypeID<T>();
        auto arch = archetype;

        for (int i = 0; i < archetype->archetypeId.size(); i++) {
            if (typeId == archetype->archetypeId.at(i)) {
                return (ComponentArray<std::remove_reference_t<T>>(
                    &archetype->byteArrays[i]));
            }
        }

        return ComponentArray<std::remove_reference_t<T>>();
    }

}  // namespace ant
