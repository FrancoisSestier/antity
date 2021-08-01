#pragma once
#include <antity/core/archetype.hpp>
#include <antity/core/archetype_map.hpp>
#include <antity/core/component_array.hpp>

namespace ant {

    template <typename... Cs>
    class archetypeView final {
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

            archetype_view_iterator(size_t index, archetypeView<Cs...>* owner)
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
                return pointer{&(owner->archetype->entities[index]),
                               &(std::get<ComponentArray<Cs>>(
                                   owner->componentArrays)[index])...};
            }

            [[nodiscard]] reference operator*() const {
                return reference{owner->archetype->entities[index],
                                 std::get<ComponentArray<Cs>>(
                                     owner->componentArrays)[index]...};
            }

           private:
            difference_type index;
            archetypeView<Cs...>* owner;
        };

        archetypeView() = default;

        archetypeView(archetype* archetype)
            : archetype(archetype),
              componentArrays(
                  std::make_tuple(get_componentArray<Cs>(archetype)...)) {}

        archetype_view_iterator begin() {
            return archetype_view_iterator{0, this};
        }
        archetype_view_iterator end() {
            return archetype_view_iterator(archetype->entities.size(), this);
        }

        archetype_view_iterator rbegin() {
            return archetype_view_iterator(archetype->entities.size(), this);
        }
        archetype_view_iterator rend() {
            return archetype_view_iterator(-1, this);
        }

       private:
        archetype* archetype;
        component_arrays componentArrays;
    };

    template <typename T, typename... Ts>
    using areT = std::conjunction<std::is_same<T, Ts>...>;

    template <typename T, typename... Ts>
    inline constexpr bool areT_v = std::conjunction_v<std::is_same<T, Ts>...>;

    template <typename... Cs>
    class archetype_mapView {
       public:
        using archetype_signature_iterator = std::vector<signature>::iterator;
        using archetype_iterator
            = archetypeView<Cs...>::archetype_view_iterator;
        class archetyep_map_iterator {
           public:
            using iterator_category = std::input_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = archetype_iterator::value_type;
            using pointer = archetype_iterator::pointer;
            using reference = archetype_iterator::reference;

            archetyep_map_iterator(
                archetype_signature_iterator archetypesignatureIterator,
                archetype_mapView* owner)
                : archetypesignatureIterator(archetypesignatureIterator),
                  owner(owner) {
                if (archetypesignatureIterator
                    != owner->archetype_map->signaturesEnd()) {
                    currentView = archetypeView<Cs...>(
                        owner->archetype_map->get(*archetypesignatureIterator));
                    archetypeViewIterator = currentView.begin();
                    archetypeViewEnd = currentView.end();
                }
            }

            archetyep_map_iterator& operator++() noexcept {
                if (++archetypeViewIterator == archetypeViewEnd) {
                    do {
                        if (++archetypesignatureIterator
                            == owner->archetype_map->signaturesEnd()) {
                            return *this;
                        }
                    } while ((owner->include & ~(*archetypesignatureIterator))
                                 .any());

                    currentView = archetypeView<Cs...>(
                        owner->archetype_map->get(*archetypesignatureIterator));
                    archetypeViewIterator = currentView.begin();
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
                return other.archetypesignatureIterator
                       == archetypesignatureIterator;
            }

            [[nodiscard]] bool operator!=(
                const archetyep_map_iterator& other) const noexcept {
                return !(*this == other);
            }

            [[nodiscard]] pointer operator->() const {
                return archetypeViewIterator.operator->();
            }

            [[nodiscard]] reference operator*() const {
                return *archetypeViewIterator;
            }

           private:
            archetypeView<Cs...> currentView;
            archetype_iterator archetypeViewIterator;
            archetype_iterator archetypeViewEnd;
            archetype_signature_iterator archetypesignatureIterator;
            archetype_mapView* owner;
        };

        archetype_mapView(archetype_map* archetype_map, signature include)
            : archetype_map(archetype_map), include(include) {}

        inline auto begin() {
            return archetyep_map_iterator{
                archetype_map->signaturesBegin(include), this};
        }

        inline auto end() {
            return archetyep_map_iterator{archetype_map->signaturesEnd(), this};
        }

       private:
        signature include;
        archetype_map* archetype_map;
    };

    template <typename... Cs>
    class MultiarchetypeView final {
        using underlying_views = typename std::vector<archetypeView<Cs...>>;

       public:
        class multi_component_view_iterator final {
           public:
            using archetype_view_iterator
                = archetypeView<Cs...>::archetype_view_iterator;
            using iterator_category = std::input_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type
                = archetypeView<Cs...>::archetype_view_iterator::value_type;
            using pointer
                = archetypeView<Cs...>::archetype_view_iterator::pointer;
            using reference
                = archetypeView<Cs...>::archetype_view_iterator::reference;

            multi_component_view_iterator(
                std::vector<archetypeView<Cs...>>::iterator
                    underlyingViewsIterator,
                archetype_view_iterator archetypeViewIterator,
                MultiarchetypeView<Cs...>* owner)
                : underlyingViewsIterator(underlyingViewsIterator),
                  archetypeViewIterator(archetypeViewIterator),
                  owner(owner) {}

            multi_component_view_iterator& operator++() noexcept {
                if (++archetypeViewIterator == underlyingViewsIterator->end()
                    && ++underlyingViewsIterator
                           != owner->underlyingViews.end()) {
                    archetypeViewIterator = underlyingViewsIterator->begin();
                }
                return *this;
            }

            multi_component_view_iterator operator++(int) noexcept {
                archetype_view_iterator tmp = *this;
                return ++(*this), tmp;
            }

            [[nodiscard]] bool operator==(
                const multi_component_view_iterator& other) const noexcept {
                return other.underlyingViewsIterator == underlyingViewsIterator;
            }

            [[nodiscard]] bool operator!=(
                const multi_component_view_iterator& other) const noexcept {
                return !(*this == other);
            }

            [[nodiscard]] pointer operator->() const {
                return archetypeViewIterator.operator->();
            }

            [[nodiscard]] reference operator*() const {
                return *archetypeViewIterator;
            }

           private:
            std::vector<archetypeView<Cs...>>::iterator underlyingViewsIterator;
            archetype_view_iterator archetypeViewIterator;
            MultiarchetypeView<Cs...>* owner;
        };

        void emplace_back(archetype* archetype) {
            underlyingViews.emplace_back(archetypeView<Cs...>(archetype));
        }

        multi_component_view_iterator begin() {
            return multi_component_view_iterator{
                underlyingViews.begin(), underlyingViews.begin()->begin(),
                this};
        }

        multi_component_view_iterator end() {
            return multi_component_view_iterator{
                underlyingViews.end(), underlyingViews.begin()->begin(), this};
        }

       private:
        underlying_views underlyingViews;
    };

    template <typename... Cs>
    MultiarchetypeView<Cs...> build_multiarchetype_view(
        archetype_id archetype_id, archetype_map* archetype_map,
        chunk_id_t chunk_id = _null_chunk) {
        MultiarchetypeView<Cs...> multiarchetypeView;
        for (auto&& archetype : *archetype_map->get()) {
            if (!std::ranges::includes(archetype.second->archetype_id.begin(),
                                       archetype.second->archetype_id.end(),
                                       archetype_id.begin(),
                                       archetype_id.end())) {
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
