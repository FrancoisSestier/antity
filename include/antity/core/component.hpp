#pragma once

#include <antity/core/identifier.hpp>
#include <antity/utility/robin_hood.hpp>
#include <iterator>
#include <new>

namespace ant {

    class component_base {
       public:
        virtual ~component_base() {}

        virtual void destroy_data(std::byte* data) const = 0;
        virtual void move_data(std::byte* source,
                               std::byte* destination) const = 0;
        virtual void construct_data(std::byte* data) const = 0;

        virtual size_t get_size() const = 0;
        virtual std::byte* allocate(size_t size) = 0;
        virtual component_id_t get() = 0;
    };

    using component_map
        = robin_hood::unordered_map<component_id_t,
                                    std::unique_ptr<component_base>>;

    template <class C>
    class Component : public component_base {
       public:
        struct component_storage_type {
            alignas(C) std::byte* data[sizeof(C)];
        };

        void destroy_data(std::byte* data) const override;
        void move_data(std::byte* src, std::byte* dst) const override;
        void construct_data(std::byte* data) const override;
        size_t get_size() const override;
        component_id_t get() override;
        std::byte* allocate(size_t size) override;
    };

    template <class C>
    void Component<C>::destroy_data(std::byte* data) const {
        C* dataLocation = std::launder(reinterpret_cast<C*>(data));

        dataLocation->~C();
    }

    template <class C>
    void Component<C>::construct_data(std::byte* data) const {
        new (&data[0]) C();
    }

    template <class C>
    void Component<C>::move_data(std::byte* source,
                                 std::byte* destination) const {
        new (&destination[0]) C(std::move(*reinterpret_cast<C*>(source)));
    }

    template <class C>
    std::size_t Component<C>::get_size() const {
        return sizeof(C);
    }

    template <class C>
    component_id_t Component<C>::get() {
        return static_cast<component_id_t>(type_id_generator::get<C>());
    }

    template <class C>
    std::byte* Component<C>::allocate(size_t size) {
        alignas(C) std::byte* componentData = new std::byte[size];
        return componentData;
    }
}  // namespace ant