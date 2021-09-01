#include <antity/core/archetype.hpp>

namespace ant {

    template <typename C>
    struct component_array {
        struct component_iterator {
            using iterator_category = std::random_access_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = C;
            using pointer = C*;    // or also value_type*
            using reference = C&;  // or also value_type&

            component_iterator() : m_ptr(nullptr) {}
            component_iterator(C* rhs) : m_ptr(rhs) {}

            pointer get() { return m_ptr; }

            reference operator*() const { return *m_ptr; }
            pointer operator->() { return m_ptr; }

            difference_type operator-(const component_iterator& rhs) {
                return m_ptr - rhs.m_ptr;
            }
            component_iterator operator+(const difference_type& rhs) {
                return component_iterator(m_ptr + rhs);
            }
            component_iterator operator-(const difference_type& rhs) {
                return component_iterator(m_ptr - rhs);
            }
            component_iterator& operator+=(const difference_type& rhs) {
                m_ptr += rhs;
                return *this;
            }
            component_iterator& operator-=(const difference_type& rhs) {
                m_ptr -= rhs;
                return *this;
            }

            reference operator[](const difference_type& rhs) {
                return m_ptr[rhs];
            }

            component_iterator& operator++() {
                m_ptr++;
                return *this;
            }
            component_iterator operator++(int) {
                component_iterator tmp = *this;
                ++(*this);
                return tmp;
            }
            component_iterator& operator--() {
                m_ptr--;
                return *this;
            }
            component_iterator operator--(int) {
                component_iterator tmp = *this;
                --(*this);
                return tmp;
            }

            bool operator>(const component_iterator& rhs) const {
                return m_ptr > rhs.m_ptr;
            }
            bool operator<(const component_iterator& rhs) const {
                return m_ptr < rhs.m_ptr;
            }
            bool operator>=(const component_iterator& rhs) const {
                return m_ptr >= rhs.m_ptr;
            }
            bool operator<=(const component_iterator& rhs) const {
                return m_ptr <= rhs.m_ptr;
            }

            friend bool operator==(const component_iterator& a,
                                   const component_iterator& b) {
                return a.m_ptr == b.m_ptr;
            };
            friend bool operator!=(const component_iterator& a,
                                   const component_iterator& b) {
                return a.m_ptr != b.m_ptr;
            };

           private:
            pointer m_ptr;
        };

        component_array() = default;

        component_array(byte_array* byteArray)
            : data(reinterpret_cast<C*>(byteArray->componentData)),
              byteArray(byteArray) {}

        const C& operator[](size_t index) const {
            return data[index];
        }

        C& operator[](size_t index) { return data[index]; }

        bool valid() const noexcept { return data != nullptr; }

        C* begin() { return data; }

        C* end() { return data + byteArray->size; }

        int16_t size() { return byteArray->size; }

        C* data{nullptr};
        byte_array* byteArray{nullptr};
    };

    template <typename C>
    auto get_array(archetype* arch) {
        return get_component_array<C>(arch);
    }

    template <typename C>
    inline auto get_component_array(archetype* arch) {
        return (component_array<std::remove_reference_t<C>>(
            &arch->byte_arrays[arch->component_id_index.at(
                type_id_generator::get<C>())]));
    }

}  // namespace ant
