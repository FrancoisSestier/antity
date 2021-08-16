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
                                    owner->component_arrays_)[value]...);
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
                                   owner->component_arrays_)[index])...};
            }

            [[nodiscard]] reference operator*() const {
                return reference{owner->archetype_->entities[index],
                                 std::get<ComponentArray<Cs>>(
                                     owner->component_arrays_)[index]...};
            }

           private:
            difference_type index;
            archetype_view<Cs...>* owner;
        };

        archetype_view() = default;

        archetype_view(archetype* arch)
            : archetype_(arch),
              component_arrays_(
                  std::make_tuple(get_component_array<Cs>(arch)...)) {}

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
        component_arrays component_arrays_;
    };

    template <typename T, typename... Ts>
    using areT = std::conjunction<std::is_same<T, Ts>...>;

    template <typename T, typename... Ts>
    inline constexpr bool areT_v = std::conjunction_v<std::is_same<T, Ts>...>;

    template <typename... Cs>
    class archetype_map_view {
       public:
        using archeytpe_key_iterator = std::vector<archetype_key>::iterator;
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
                archeytpe_key_iterator arch_signature_iterator,
                archetype_map_view* owner)
                : archetype_signature_iterator_(arch_signature_iterator),
                  owner(owner) {
                if (archetype_signature_iterator_
                    != owner->archetype_map_->signatures_end()) {
                    current_view_ = archetype_view<Cs...>(
                        owner->archetype_map_->get(*archetype_signature_iterator_));
                    archetype_view_iterator_ = current_view_.begin();
                    archetype_view_end_ = current_view_.end();
                }
            }

            archetyep_map_iterator& operator++() noexcept {
                if (++archetype_view_iterator_ == archetype_view_end_) {
                    do {
                        if (++archetype_signature_iterator_
                            == owner->archetype_map_->signatures_end()) {
                            return *this;
                        }
                    } while ((owner->include_ & ~(*archetype_signature_iterator_).signature)
                                 .any());

                    current_view_ = archetype_view<Cs...>(
                        owner->archetype_map_->get(*archetype_signature_iterator_));
                    archetype_view_iterator_ = current_view_.begin();
                    archetype_view_end_ = current_view_.end();
                }
                return *this;
            }

            archetyep_map_iterator operator++(int) noexcept {
                archetyep_map_iterator tmp = *this;
                return ++(*this), tmp;
            }

            [[nodiscard]] bool operator==(
                const archetyep_map_iterator& other) const noexcept {
                return other.archetype_signature_iterator_
                       == archetype_signature_iterator_;
            }

            [[nodiscard]] bool operator!=(
                const archetyep_map_iterator& other) const noexcept {
                return !(*this == other);
            }

            [[nodiscard]] pointer operator->() const {
                return archetype_view_iterator_.operator->();
            }

            [[nodiscard]] reference operator*() const {
                return *archetype_view_iterator_;
            }

           private:
            archetype_view<Cs...> current_view_;
            archetype_iterator archetype_view_iterator_;
            archetype_iterator archetype_view_end_;
            archeytpe_key_iterator archetype_signature_iterator_;
            archetype_map_view* owner;
        };

        archetype_map_view(archetype_map* arch_map, signature_t include, chunk_id_t chunk_id)
            : archetype_map_(arch_map), include_(include), chunk_id_(chunk_id) {}

        inline auto begin() {
            return archetyep_map_iterator{
                archetype_map_->signatures_begin(archetype_key{include_,chunk_id_}), this};
        }

        inline auto end() {
            return archetyep_map_iterator{archetype_map_->signatures_end(), this};
        }

       private:
        chunk_id_t chunk_id_;
        signature_t include_;
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
                    underlying_view_iterator_,
                archetype_view_iterator arch_view_iterator,
                multi_archetype_view<Cs...>* owner)
                : underlying_view_iterator_(underlying_view_iterator_),
                  archetype_view_iterator_(arch_view_iterator),
                  owner_(owner) {}

            multi_component_view_iterator& operator++() noexcept {
                if (++archetype_view_iterator_ == underlying_view_iterator_->end()
                    && ++underlying_view_iterator_
                           != owner_->underlying_views_.end()) {
                    archetype_view_iterator_ = underlying_view_iterator_->begin();
                }
                return *this;
            }

            multi_component_view_iterator operator++(int) noexcept {
                archetype_view_iterator tmp = *this;
                return ++(*this), tmp;
            }

            [[nodiscard]] bool operator==(
                const multi_component_view_iterator& other) const noexcept {
                return other.underlying_view_iterator_ == underlying_view_iterator_;
            }

            [[nodiscard]] bool operator!=(
                const multi_component_view_iterator& other) const noexcept {
                return !(*this == other);
            }

            [[nodiscard]] pointer operator->() const {
                return archetype_view_iterator_.operator->();
            }

            [[nodiscard]] reference operator*() const {
                return *archetype_view_iterator_;
            }

           private:
            std::vector<archetype_view<Cs...>>::iterator underlying_view_iterator_;
            archetype_view_iterator archetype_view_iterator_;
            multi_archetype_view<Cs...>* owner_;
        };

        void emplace_back(archetype* archetype) {
            underlying_views_.emplace_back(archetype_view<Cs...>(archetype));
        }

        multi_component_view_iterator begin() {
            return multi_component_view_iterator{
                underlying_views_.begin(), underlying_views_.begin()->begin(),
                this};
        }

        multi_component_view_iterator end() {
            return multi_component_view_iterator{
                underlying_views_.end(), underlying_views_.begin()->begin(), this};
        }

       private:
        underlying_views underlying_views_;
    };

    template <typename... Cs>
    multi_archetype_view<Cs...> build_multiarchetype_view(
        component_id_list component_id_list, archetype_map* archetype_map,
        chunk_id_t chunk_id = _null_chunk) {
        multi_archetype_view<Cs...> multiarchetypeView;
        for (auto&& archetype : *archetype_map->get()) {
            if (!std::ranges::includes(archetype.second->component_ids.begin(),
                                       archetype.second->component_ids.end(),
                                       component_id_list.begin(),
                                       component_id_list.end())) {
                continue;
            }
            if (archetype.second->key.chunk_id != chunk_id
                && chunk_id != _null_chunk) {
                continue;
            }
            multiarchetypeView.emplace_back(archetype.second.get());
        }
        return multiarchetypeView;
    }
}  // namespace ant
