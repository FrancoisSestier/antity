#include <antity/core/archetype.hpp>

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
            ComponentIterator(const ComponentIterator& rhs)
                : m_ptr(rhs.m_ptr) {}

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

            friend bool operator==(const ComponentIterator& a,
                                   const ComponentIterator& b) {
                return a.m_ptr == b.m_ptr;
            };
            friend bool operator!=(const ComponentIterator& a,
                                   const ComponentIterator& b) {
                return a.m_ptr != b.m_ptr;
            };

           private:
            pointer m_ptr;
        };
        ComponentArray() = default;

        ComponentArray(byte_array* byteArray)
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
        byte_array* byteArray{nullptr};
    };

    template <typename T>
    inline auto get_componentArray(archetype* archetype) {
        for (int i = 0; i < archetype->archetype_id.size(); i++) {
            if (type_id_generator::get<T>() == archetype->archetype_id.at(i)) {
                return (ComponentArray<std::remove_reference_t<T>>(
                    &archetype->byte_arrays[i]));
            }
        }

        return ComponentArray<std::remove_reference_t<T>>();
    }

}  // namespace ant
