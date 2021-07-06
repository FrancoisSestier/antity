#pragma once
#include "Archetype.h"
#include "ArchetypeMap.h"
#include "ComponentArray.h"

namespace ant {

    template <typename... Cs>
    class ArchetypeView final {
        using component_arrays = std::tuple<ComponentArray<Cs>...>;

       public:
        class archetype_view_iterator final {
           public:
            using iterator_category = std::random_access_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = std::tuple<Entity, Cs...>;
            using pointer = std::tuple<Entity*, Cs*...>;
            using reference = std::tuple<Entity&, Cs&...>;

            archetype_view_iterator() = default;

            archetype_view_iterator(size_t index, ArchetypeView<Cs...>* owner)
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
            ArchetypeView<Cs...>* owner;
        };

        ArchetypeView() = default;

        ArchetypeView(Archetype* archetype)
            : archetype(archetype),
              componentArrays(
                  std::make_tuple(GetComponentArray<Cs>(archetype)...)) {}

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
        Archetype* archetype;
        component_arrays componentArrays;
    };

    template <typename T, typename... Ts>
    using areT = std::conjunction<std::is_same<T, Ts>...>;

    template <typename T, typename... Ts>
    inline constexpr bool areT_v = std::conjunction_v<std::is_same<T, Ts>...>;

    template <typename... Cs>
    class ArchetypeMapView {
       public:
        using archetype_signature_iterator = std::vector<Signature>::iterator;
        using archetype_iterator
            = ArchetypeView<Cs...>::archetype_view_iterator;
        class archetyep_map_iterator {
           public:
            using iterator_category = std::input_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = archetype_iterator::value_type;
            using pointer = archetype_iterator::pointer;
            using reference = archetype_iterator::reference;

            archetyep_map_iterator(
                archetype_signature_iterator archetypeSignatureIterator,
                ArchetypeMapView* owner)
                : archetypeSignatureIterator(archetypeSignatureIterator),
                  owner(owner) {
                if (archetypeSignatureIterator
                    != owner->archetypeMap->signaturesEnd()) {
                    currentView
                        = ArchetypeView<Cs...>(owner->archetypeMap->GetArchetype(
                            *archetypeSignatureIterator));
                    archetypeViewIterator = currentView.begin();
                    archetypeViewEnd = currentView.end();
                }
            }

            archetyep_map_iterator& operator++() noexcept {
                if (++archetypeViewIterator == archetypeViewEnd) {
                    do {
                        if (++archetypeSignatureIterator
                            == owner->archetypeMap->signaturesEnd()) {
                            return *this;
                        }
                    } while ((owner->include
                             & ~(*archetypeSignatureIterator)).any());


                    currentView
                        = ArchetypeView<Cs...>(owner->archetypeMap->GetArchetype(*archetypeSignatureIterator));
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
                return other.archetypeSignatureIterator
                       == archetypeSignatureIterator;
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
            ArchetypeView<Cs...> currentView;
            archetype_iterator archetypeViewIterator;
            archetype_iterator archetypeViewEnd;
            archetype_signature_iterator archetypeSignatureIterator;
            ArchetypeMapView* owner;
        };

        ArchetypeMapView(ArchetypeMap* archetypeMap, Signature include)
            : archetypeMap(archetypeMap), include(include) {}

        inline auto begin() {
            return archetyep_map_iterator{
                archetypeMap->signaturesBegin(include), this};
        }

        inline auto end() {
            return archetyep_map_iterator{archetypeMap->signaturesEnd(), this};
        }

       private:
        Signature include;
        ArchetypeMap* archetypeMap;
    };

    template <typename... Cs>
    class MultiArchetypeView final {
        using underlying_views = typename std::vector<ArchetypeView<Cs...>>;

       public:
        class multi_component_view_iterator final {
           public:
            using archetype_view_iterator
                = ArchetypeView<Cs...>::archetype_view_iterator;
            using iterator_category = std::input_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type
                = ArchetypeView<Cs...>::archetype_view_iterator::value_type;
            using pointer
                = ArchetypeView<Cs...>::archetype_view_iterator::pointer;
            using reference
                = ArchetypeView<Cs...>::archetype_view_iterator::reference;

            multi_component_view_iterator(
                std::vector<ArchetypeView<Cs...>>::iterator
                    underlyingViewsIterator,
                archetype_view_iterator archetypeViewIterator,
                MultiArchetypeView<Cs...>* owner)
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
            std::vector<ArchetypeView<Cs...>>::iterator underlyingViewsIterator;
            archetype_view_iterator archetypeViewIterator;
            MultiArchetypeView<Cs...>* owner;
        };

        void emplace_back(Archetype* archetype) {
            underlyingViews.emplace_back(ArchetypeView<Cs...>(archetype));
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
    MultiArchetypeView<Cs...> build_multiarchetype_view(
        ArchetypeID archetypeId, ArchetypeMap* archetypeMap,
        ChunkID chunkId = NULL_CHUNK) {
        MultiArchetypeView<Cs...> multiArchetypeView;
        for (auto&& archetype : *archetypeMap->get()) {
            if (!std::ranges::includes(archetype.second->archetypeId.begin(),
                                       archetype.second->archetypeId.end(),
                                       archetypeId.begin(),
                                       archetypeId.end())) {
                continue;
            }
            if (archetype.second->chunkId != chunkId && chunkId != NULL_CHUNK) {
                continue;
            }
            multiArchetypeView.emplace_back(archetype.second.get());
        }
        return multiArchetypeView;
    }
}  // namespace ant
