#pragma once
#include <antity/core/archetype.hpp>
#include <antity/core/archetype_map.hpp>
#include <antity/core/component_array.hpp>

namespace ant {

    template <typename... Cs>
    class archetype_view final {
        using component_arrays = std::tuple<ComponentArray<Cs>...>;

       public:
        class archetype_view_iterator final {
           public:
            using iterator_category = std::random_access_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = std::tuple<entity_t, Cs...>;
            using pointer = std::tuple<entity_t*, Cs*...>;
            using reference = std::tuple<entity_t&, Cs&...>;

            archetype_view_iterator() = default;

            archetype_view_iterator(size_t index, archetype_view<Cs...>* owner)
                : index(index), owner(owner) {}

            archetype_view_iterator& operator++() noexcept {
                return ++index, *this;
            }

            archetype_view_iterator operator++(int) noexcept {
                archetype_view_iterator tmp = *this;
                return ++(*this), tmp;
            }

            archetype_view_iterator& operator--() noexcept {
                return ++index, *this;
            }

            archetype_view_iterator operator--(int) noexcept {
                archetype_view_iterator tmp = *this;
                return operator--(), tmp;
            }

            archetype_view_iterator& operator+=(
                const difference_type value) noexcept {
                index -= value;
                return *this;
            }

            archetype_view_iterator operator+(
                const difference_type value) const noexcept {
                archetype_view_iterator copy = *this;
                return (copy += value);
            }

            archetype_view_iterator& operator-=(
                const difference_type value) noexcept {
                return (*this += -value);
            }

            archetype_view_iterator operator-(
                const difference_type value) const noexcept {
                return (*this + -value);
            }

            difference_type operator-(
                const archetype_view_iterator& other) const noexcept {
                return other.index - index;
            }

            [[nodiscard]] reference operator[](
                const difference_type value) const {
                return std::tie(owner->archetype->entities[value],
                                std::get<ComponentArray<Cs>>(
                                    owner->componentArrays)[value]...);
            }

            [[nodiscard]] bool operator==(
                const archetype_view_iterator& other) const noexcept {
                return other.index == index;
            }

            [[nodiscard]] bool operator!=(
                const archetype_view_iterator& other) const noexcept {
                return !(*this == other);
            }

            [[nodiscard]] bool operator<(
                const archetype_view_iterator& other) const noexcept {
                return index > other.index;
            }

            [[nodiscard]] bool operator>(
                const archetype_view_iterator& other) const noexcept {
                return index < other.index;
            }

            [[nodiscard]] bool operator<=(
                const archetype_view_iterator& other) const noexcept {
                return !(*this > other);
            }

            [[nodiscard]] bool operator>=(
                const archetype_view_iterator& other) const noexcept {
                return !(*this < other);
            }

            [[nodiscard]] pointer operator->() const {
                return pointer{&(owner->archetype_->entities[index]),
                               &(std::get<ComponentArray<Cs>>(
                                   owner->componentArrays)[index])...};
            }

            [[nodiscard]] reference operator*() const {
                return reference{owner->archetype_->entities[index],
                                 std::get<ComponentArray<Cs>>(
                                     owner->componentArrays)[index]...};
            }

           private:
            difference_type index;
            archetype_view<Cs...>* owner;
        };

        archetype_view() = default;

        archetype_view(archetype* arch)
            : archetype_(arch),
              componentArrays(
                  std::make_tuple(get_componentArray<Cs>(arch)...)) {}

        archetype_view_iterator begin() {
            return archetype_view_iterator{0, this};
        }
        archetype_view_iterator end() {
            return archetype_view_iterator(archetype_->entities.size(), this);
        }

        archetype_view_iterator rbegin() {
            return archetype_view_iterator(archetype_->entities.size(), this);
        }
        archetype_view_iterator rend() {
            return archetype_view_iterator(-1, this);
        }

       private:
        archetype* archetype_;
        component_arrays componentArrays;
    };

    template <typename T, typename... Ts>
    using areT = std::conjunction<std::is_same<T, Ts>...>;

    template <typename T, typename... Ts>
    inline constexpr bool areT_v = std::conjunction_v<std::is_same<T, Ts>...>;

    template <typename... Cs>
    class archetype_map_view {
       public:
        using archetype_signature_iterator = std::vector<signature_t>::iterator;
        using archetype_iterator
            = archetype_view<Cs...>::archetype_view_iterator;
        class archetyep_map_iterator {
           public:
            using iterator_category = std::input_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = archetype_iterator::value_type;
            using pointer = archetype_iterator::pointer;
            using reference = archetype_iterator::reference;

            archetyep_map_iterator(
                archetype_signature_iterator archetype_signature_iterator,
                archetype_map_view* owner)
                : archetype_signature_iterator(archetype_signature_iterator),
                  owner(owner) {
                if (archetype_signature_iterator
                    != owner->archetype_map_->signatures_end()) {
                    currentView = archetype_view<Cs...>(
                        owner->archetype_map_->get(*archetype_signature_iterator));
                    archetype_view_iterator = currentView.begin();
                    archetypeViewEnd = currentView.end();
                }
            }

            archetyep_map_iterator& operator++() noexcept {
                if (++archetype_view_iterator == archetypeViewEnd) {
                    do {
                        if (++archetype_signature_iterator
                            == owner->archetype_map_->signatures_end()) {
                            return *this;
                        }
                    } while ((owner->include & ~(*archetype_signature_iterator))
                                 .any());

                    currentView = archetype_view<Cs...>(
                        owner->archetype_map_->get(*archetype_signature_iterator));
                    archetype_view_iterator = currentView.begin();
                    archetypeViewEnd = currentView.end();
                }
                return *this;
            }

            archetyep_map_iterator operator++(int) noexcept {
                archetyep_map_iterator tmp = *this;
                return ++(*this), tmp;
            }

            [[nodiscard]] bool operator==(
                const archetyep_map_iterator& other) const noexcept {
                return other.archetype_signature_iterator
                       == archetype_signature_iterator;
            }

            [[nodiscard]] bool operator!=(
                const archetyep_map_iterator& other) const noexcept {
                return !(*this == other);
            }

            [[nodiscard]] pointer operator->() const {
                return archetype_view_iterator.operator->();
            }

            [[nodiscard]] reference operator*() const {
                return *archetype_view_iterator;
            }

           private:
            archetype_view<Cs...> currentView;
            archetype_iterator archetype_view_iterator;
            archetype_iterator archetypeViewEnd;
            archetype_signature_iterator archetype_signature_iterator;
            archetype_map_view* owner;
        };

        archetype_map_view(archetype_map* arch_map, signature_t include)
            : archetype_map_(arch_map), include(include) {}

        inline auto begin() {
            return archetyep_map_iterator{
                archetype_map_->signaturesBegin(include), this};
        }

        inline auto end() {
            return archetyep_map_iterator{archetype_map_->signatures_end(), this};
        }

       private:
        signature_t include;
        archetype_map* archetype_map_;
    };

    template <typename... Cs>
    class multi_archetype_view final {
        using underlying_views = typename std::vector<archetype_view<Cs...>>;

       public:
        class multi_component_view_iterator final {
           public:
            using archetype_view_iterator
                = archetype_view<Cs...>::archetype_view_iterator;
            using iterator_category = std::input_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type
                = archetype_view<Cs...>::archetype_view_iterator::value_type;
            using pointer
                = archetype_view<Cs...>::archetype_view_iterator::pointer;
            using reference
                = archetype_view<Cs...>::archetype_view_iterator::reference;

            multi_component_view_iterator(
                std::vector<archetype_view<Cs...>>::iterator
                    underlying_view_iterator,
                archetype_view_iterator archetype_view_iterator,
                multi_archetype_view<Cs...>* owner)
                : underlying_view_iterator(underlying_view_iterator),
                  archetype_view_iterator(archetype_view_iterator),
                  owner(owner) {}

            multi_component_view_iterator& operator++() noexcept {
                if (++archetype_view_iterator == underlying_view_iterator->end()
                    && ++underlying_view_iterator
                           != owner->underlying_views.end()) {
                    archetype_view_iterator = underlying_view_iterator->begin();
                }
                return *this;
            }

            multi_component_view_iterator operator++(int) noexcept {
                archetype_view_iterator tmp = *this;
                return ++(*this), tmp;
            }

            [[nodiscard]] bool operator==(
                const multi_component_view_iterator& other) const noexcept {
                return other.underlying_view_iterator == underlying_view_iterator;
            }

            [[nodiscard]] bool operator!=(
                const multi_component_view_iterator& other) const noexcept {
                return !(*this == other);
            }

            [[nodiscard]] pointer operator->() const {
                return archetype_view_iterator.operator->();
            }

            [[nodiscard]] reference operator*() const {
                return *archetype_view_iterator;
            }

           private:
            std::vector<archetype_view<Cs...>>::iterator underlying_view_iterator;
            archetype_view_iterator archetype_view_iterator;
            multi_archetype_view<Cs...>* owner;
        };

        void emplace_back(archetype* archetype) {
            underlying_views.emplace_back(archetype_view<Cs...>(archetype));
        }

        multi_component_view_iterator begin() {
            return multi_component_view_iterator{
                underlying_views.begin(), underlying_views.begin()->begin(),
                this};
        }

        multi_component_view_iterator end() {
            return multi_component_view_iterator{
                underlying_views.end(), underlying_views.begin()->begin(), this};
        }

       private:
        underlying_views underlying_views;
    };

    template <typename... Cs>
    multi_archetype_view<Cs...> build_multiarchetype_view(
        archetype_id_t archetype_id_t, archetype_map* archetype_map,
        chunk_id_t chunk_id = _null_chunk) {
        multi_archetype_view<Cs...> multiarchetypeView;
        for (auto&& archetype : *archetype_map->get()) {
            if (!std::ranges::includes(archetype.second->archetype_id_t.begin(),
                                       archetype.second->archetype_id_t.end(),
                                       archetype_id_t.begin(),
                                       archetype_id_t.end())) {
                continue;
            }
            if (archetype.second->chunk_id != chunk_id
                && chunk_id != _null_chunk) {
                continue;
            }
            multiarchetypeView.emplace_back(archetype.second.get());
        }
        return multiarchetypeView;
    }
}  // namespace ant
