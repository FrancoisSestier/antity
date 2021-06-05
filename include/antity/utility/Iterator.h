#pragma once
#include <chrono>
#include <tuple>
#include <vector>

#include "TypeTraits.h"

template <typename... T, size_t... I>
inline auto makeReferencesHelper(std::tuple<T...>& t,
                                 std::index_sequence<I...>) {
    return std::tie(*std::get<I>(t)...);
}

template <typename... T>
inline auto makeReferences(std::tuple<T...>& t) {
    return makeReferencesHelper<T...>(t,
                                      std::make_index_sequence<sizeof...(T)>{});
}

template <typename C>
struct Iterator {
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = C;
    using pointer = C*;    // or also value_type*
    using reference = C&;  // or also value_type&

    Iterator() : m_ptr(nullptr) {}
    Iterator(C* rhs) : m_ptr(rhs) {}
    Iterator(const Iterator& rhs) : m_ptr(rhs.m_ptr) {}

    pointer get() { return m_ptr; }

    reference operator*() const { return *m_ptr; }
    pointer operator->() { return m_ptr; }

    difference_type operator-(const Iterator& rhs) { return m_ptr - rhs.m_ptr; }
    Iterator operator+(const difference_type& rhs) {
        return Iterator(m_ptr + rhs);
    }
    Iterator operator-(const difference_type& rhs) {
        return Iterator(m_ptr - rhs);
    }
    Iterator& operator+=(const difference_type& rhs) {
        m_ptr += rhs;
        return *this;
    }
    Iterator& operator-=(const difference_type& rhs) {
        m_ptr -= rhs;
        return *this;
    }

    reference operator[](const difference_type& rhs) { return m_ptr[rhs]; }

    Iterator& operator++() {
        m_ptr++;
        return *this;
    }
    Iterator operator++(int) {
        Iterator tmp = *this;
        ++(*this);
        return tmp;
    }
    Iterator& operator--() {
        m_ptr--;
        return *this;
    }
    Iterator operator--(int) {
        Iterator tmp = *this;
        --(*this);
        return tmp;
    }

    bool operator>(const Iterator& rhs) const { return m_ptr > rhs.m_ptr; }
    bool operator<(const Iterator& rhs) const { return m_ptr < rhs.m_ptr; }
    bool operator>=(const Iterator& rhs) const { return m_ptr >= rhs.m_ptr; }
    bool operator<=(const Iterator& rhs) const { return m_ptr <= rhs.m_ptr; }

    friend bool operator==(const Iterator& a, const Iterator& b) {
        return a.m_ptr == b.m_ptr;
    };
    friend bool operator!=(const Iterator& a, const Iterator& b) {
        return a.m_ptr != b.m_ptr;
    };

   private:
    pointer m_ptr;
};

template <typename It>
struct MultiIterator {
    using difference_type = typename std::iterator_traits<It>::difference_type;
    using value_type = typename std::iterator_traits<It>::value_type;
    using pointer = typename std::iterator_traits<It>::pointer;
    using reference = typename std::iterator_traits<It>::reference;
    using iterator_category =
        typename std::iterator_traits<It>::iterator_category;

    MultiIterator(It iterator, std::vector<It>* begins, std::vector<It>* ends,
                  size_t index = 0)
        : iterator(iterator), begins(begins), ends(ends), index(index){};

    reference operator*() const { return *iterator; }
    pointer operator->() { return iterator; }

    MultiIterator& operator++() {
        if (++iterator == ends->at(index) && index != ends->size() - 1) {
            ++index;
            iterator = begins->at(index);
        }

        return *this;
    }

    MultiIterator operator++(int) {
        MultiIterator tmp = *this;
        this->operator++();
        return tmp;
    }

    MultiIterator& operator--() {
        if (iterator == begins->at(index) && index != 0) {
            --index;
            iterator == ends(index);
        }
        --iterator;
        return *this;
    }

    MultiIterator operator--(int) {
        MultiIterator tmp = *this;
        this->operator--();
        return tmp;
    }

    difference_type operator-(const MultiIterator& rhs) {
        if (rhs.index == index) {
            return iterator - rhs.iterator;
        }
        difference_type diff = 0;
        diff += rhs.ends->at(rhs.index) - rhs.iterators.at(rhs.index);
        diff += (iterator - begins->at(index));
        for (size_t i = rhs.index + 1; i < this->index; ++i) {
            diff += (ends->at(i) - begins->at(i));
        }
        return diff;
    }

    MultiIterator operator+(const difference_type& rhs) {
        MultiIterator tmp = *this;
        tmp += rhs;
        return std::move(tmp);
    }

    MultiIterator operator-(const difference_type& rhs) {
        MultiIterator tmp = *this;
        tmp -= rhs;
        return std::move(tmp);
    }

    MultiIterator& operator+=(const difference_type& rhs) {
        if (index != ends->size() - 1 && (iterator + rhs) >= ends->at(index)) {
            auto diff = rhs - (ends->at(index) - iterator);
            iterator = ends->at(index) - 1;
            index++;
            return *this += diff;
        }
        iterator += rhs;
        return *this;
    }

    MultiIterator& operator-=(const difference_type& rhs) {
        if (index != 0 && (iterator - rhs) < begins->at(index)) {
            auto diff = rhs - (iterator - begins->at(index));
            iterator = begins->at(index);
            index--;
            return *this -= diff;
        }
        iterator -= rhs;
        return *this;
    }

    reference operator[](const difference_type& rhs) { return *(*this + rhs); }

    bool operator>(const MultiIterator& rhs) const {
        if (index > rhs.index) {
            return true;
        }
        if (index == rhs.index) {
            return iterator > rhs.iterator;
        }
        return false;
    }

    bool operator<(const MultiIterator& rhs) const {
        if (index < rhs.index) {
            return true;
        }
        if (index == rhs.index) {
            return iterator < rhs.iterator;
        }
        return false;
    }

    bool operator>=(const MultiIterator& rhs) const {
        if (index > rhs.index) {
            return true;
        }
        if (index == rhs.index) {
            return iterator >= rhs.iterator;
        }
        return false;
    }
    bool operator<=(const MultiIterator& rhs) const {
        if (index < rhs.index) {
            return true;
        }
        if (index == rhs.index) {
            return iterator <= rhs.iterator;
        }
        return false;
    }

    friend bool operator==(const MultiIterator& a, const MultiIterator& b) {
        return (a.iterator == b.iterator);
    };

    friend bool operator!=(const MultiIterator& a, const MultiIterator& b) {
        return a.iterator != b.iterator;
    };

   public:
    size_t index = 0;
    It iterator;
    std::vector<It>* begins;
    std::vector<It>* ends;
};

template <typename It, typename... Cs>
struct ZipIterator {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = std::tuple<Cs...>;
    using pointer = std::tuple<Cs...>*;
    using reference = std::tuple<Cs&...>;

    ZipIterator(It cs) : iterators(cs) {}

    reference operator*() { return makeReferences(iterators); }

    pointer operator->() { return makePointerFromIterator(iterators); }

    ZipIterator& operator+=(const difference_type& rhs) {
        std::apply([&](auto&... tupleArgs) { ((tupleArgs += rhs), ...); },
                   iterators);
        return *this;
    }

    ZipIterator& operator-=(const difference_type& rhs) {
        std::apply([&](auto&... tupleArgs) { ((tupleArgs -= rhs), ...); },
                   iterators);
        return *this;
    }

    difference_type operator-(const ZipIterator& rhs) {
        return std::get<0>(this->iterators) - std::get<0>(rhs.iterators);
    }

    ZipIterator operator+(const difference_type& rhs) {
        ZipIterator tmp = *this;
        tmp += rhs;
        return std::move(tmp);
    }

    ZipIterator operator-(const difference_type& rhs) {
        ZipIterator tmp = *this;
        tmp -= rhs;
        return std::move(tmp);
    }

    // Prefix increment
    ZipIterator& operator++() {
        std::apply([](auto&... tupleArgs) { (tupleArgs++, ...); }, iterators);

        return *this;
    }

    // Postfix increment
    ZipIterator operator++(int) {
        It tmp = *this;
        this->operator++();
        return tmp;
    }

    // Prefix increment
    ZipIterator& operator--() {
        std::apply([](auto&... tupleArgs) { (tupleArgs--, ...); }, iterators);

        return *this;
    }

    // Postfix increment
    ZipIterator operator--(int) {
        ZipIterator tmp = *this;
        this->operator--();
        return tmp;
    }

    reference operator[](const difference_type& rhs) { return *(*this + rhs); }

    bool operator>(const ZipIterator& rhs) const {
        return std::get<0>(this->iterators) > std::get<0>(rhs.iterators);
    }

    bool operator<(const ZipIterator& rhs) const {
        return std::get<0>(this->iterators) < std::get<0>(rhs.iterators);
    }

    bool operator>=(const ZipIterator& rhs) const {
        return std::get<0>(this->iterators) >= std::get<0>(rhs.iterators);
    }

    bool operator<=(const ZipIterator& rhs) const {
        return std::get<0>(this->iterators) <= std::get<0>(rhs.iterators);
    }

    friend bool operator==(const ZipIterator& a, const ZipIterator& b) {
        return a.iterators == b.iterators;
    }

    friend bool operator!=(const ZipIterator& a, const ZipIterator& b) {
        return std::get<0>(a.iterators) != std::get<0>(b.iterators);
    };

   private:
    It iterators;
};

template <typename... Cs>
struct MultiZipIterator {
    using type = ZipIterator<std::tuple<MultiIterator<Iterator<Cs>>...>, Cs...>;
};