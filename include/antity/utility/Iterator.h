#pragma once
#include <vector>
#include <tuple>
#include "TypeTraits.h"

template<typename ...T, size_t... I>
auto makeReferencesHelper(std::tuple<T...>& t, std::index_sequence<I...>)
{
	return std::tie(*std::get<I>(t)...);
}

template<typename ...T>
auto makeReferences(std::tuple<T...>& t) {
	return makeReferencesHelper<T...>(t, std::make_index_sequence<sizeof...(T)>{});
}

template<typename C>
struct Iterator
{
	
	using iterator_category = std::random_access_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using value_type = C;
	using pointer = C*;  // or also value_type*
	using reference = C&;  // or also value_type&

	Iterator() : m_ptr(nullptr) {}
	Iterator(C* rhs) : m_ptr(rhs) {  }
	Iterator(const Iterator& rhs) : m_ptr(rhs.m_ptr) {}
	
	pointer get() { return m_ptr; }
	
	reference operator*() const { return *m_ptr; }
	pointer operator->() { return m_ptr; }


	difference_type operator-(const Iterator& rhs) { return m_ptr - rhs.m_ptr; }
	Iterator operator+(const difference_type& rhs) { return Iterator(m_ptr + rhs); }
	Iterator operator-(const difference_type& rhs) { return Iterator(m_ptr - rhs); }
	Iterator& operator+=(const difference_type& rhs) { m_ptr += rhs; return *this; }
	Iterator& operator-=(const difference_type& rhs) { m_ptr -= rhs; return *this; }
	
	reference operator[](const difference_type& rhs) { return m_ptr[rhs]; }
	
	Iterator& operator++() { m_ptr++; return *this; }
	Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
	Iterator& operator--() { m_ptr--; return *this; }
	Iterator operator--(int) { Iterator tmp = *this; --(*this); return tmp; }

	
	bool operator>(const Iterator& rhs) const { return m_ptr > rhs.m_ptr; }
	bool operator<(const Iterator& rhs) const { return m_ptr < rhs.m_ptr; }
	bool operator>=(const Iterator& rhs) const { return m_ptr >= rhs.m_ptr; }
	bool operator<=(const Iterator& rhs) const { return m_ptr <= rhs.m_ptr; }

	

	friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
	friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; };

private:

	pointer m_ptr;
};


template<typename C,typename It>
struct MultiIterator
{
	using iterator_category = std::random_access_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using value_type = C;
	using pointer = C*;  // or also value_type*
	using reference = C&;  // or also value_type&

	MultiIterator(std::vector<It> iterator, std::vector<It>* begins, std::vector<It>* ends,size_t index = 0) : iterators(iterator), begins(begins), ends(ends), index(index) {};

	reference operator*() const { return *iterators.at(index); }
	pointer operator->() { return iterators.at(index); }


	
	MultiIterator& operator++() {
		if(++iterators.at(index) == ends->at(index) && index != ends->size()-1 )
		{
			index++;
		}
		return *this;
	}

	MultiIterator operator++(int)
	{
		MultiIterator tmp = *this;
		this->operator++();
		return tmp;
	}

	MultiIterator& operator--() {
		if (iterators.at(index) == begins->at(index) && index != 0)
		{
			index--;
		}
		--iterators.at(index);
		return *this;
	}

	MultiIterator operator--(int)
	{
		MultiIterator tmp = *this;
		this->operator--();
		return tmp;
	}
	
	difference_type operator-(const MultiIterator& rhs)
	{
		if (rhs.index == index)
		{
			return iterators.at(index) - rhs.iterators.at(index);
		}
		difference_type diff = 0;
		diff += rhs.ends->at(rhs.index) - rhs.iterators.at(rhs.index);
		diff += (iterators.at(index) - begins->at(index));
		for(size_t i = rhs.index + 1; i < this->index;++i)
		{
			diff += (ends->at(i) - begins->at(i));
		}
		return diff;
	}
	
	MultiIterator operator+(const difference_type& rhs)
	{
		MultiIterator tmp = *this;
		tmp += rhs;
		return std::move(tmp);
	}

	

	MultiIterator operator-(const difference_type& rhs)
	{
		MultiIterator tmp = *this;
		tmp -= rhs;
		return std::move(tmp);
	}

	
	MultiIterator& operator+=(const difference_type& rhs)
	{
		if(index != ends->size() - 1 && (iterators.at(index) + rhs) >= ends->at(index))
		{
			auto diff = rhs - (ends->at(index) - iterators.at(index));
			iterators.at(index) = ends->at(index) - 1;
			index++;
			return *this += diff;
		}
		iterators.at(index) += rhs;
		return *this;
	}
	
	MultiIterator& operator-=(const difference_type& rhs)
	{
		if ( index != 0 && (iterators.at(index) - rhs) < begins->at(index) )
		{
			auto diff = rhs - (iterators.at(index) - begins->at(index));
			iterators.at(index) = begins->at(index);
			index--;
			return *this -= diff;
		}
		iterators.at(index) -= rhs;
		return *this;
	}

	reference operator[](const difference_type& rhs)
	{
		return *(*this + rhs);
	}

	bool operator>(const MultiIterator& rhs) const
	{
		if(index > rhs.index)
		{
			return true;
		}
		if(index == rhs.index)
		{
			return iterators.at(index) > rhs.iterators.at(index);
		}
		return false;
		
	}
	
	bool operator<(const MultiIterator& rhs) const {
		if (index < rhs.index)
		{
			return true;
		}
		if (index == rhs.index)
		{
			return iterators.at(index) < rhs.iterators.at(index);
		}
		return false;
	}
	
	bool operator>=(const MultiIterator& rhs) const {
		if (index > rhs.index)
		{
			return true;
		}
		if (index == rhs.index)
		{
			return iterators.at(index) >= rhs.iterators.at(index);
		}
		return false;
	}
	bool operator<=(const MultiIterator& rhs) const {
		if (index < rhs.index)
		{
			return true;
		}
		if (index == rhs.index)
		{
			return iterators.at(index) <= rhs.iterators.at(index);
		}
		return false;
	}
	
	friend bool operator== (const MultiIterator& a, const MultiIterator& b)
	{
		if(a.iterators.size() == 0 && b.iterators.size()==0)
		{
			return true;
		}
		return (a.iterators.at(a.index) == b.iterators.at(a.index));
	};
	
	friend bool operator!= (const MultiIterator& a, const MultiIterator& b) {
		if (a.iterators.empty() && b.iterators.empty())
		{
			return false;
		}
		return (a.index != b.index || a.iterators.at(a.index) != b.iterators.at(a.index));
	};

public:
	size_t index = 0;
	std::vector<It> iterators;
	std::vector<It>* begins;
	std::vector<It>* ends;
};

template<typename  It, typename ...Cs>
struct ZipIterator
{
	using iterator_category = std::forward_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using value_type = std::tuple <Cs...>;
	using pointer = std::tuple <Cs...>*;  // or also value_type*
	using reference = std::tuple <Cs&...>;  // or also value_type&

	ZipIterator(It cs) : iterators(cs){}
	
	reference operator*()
	{
		return makeReferences(iterators);
	}

	pointer operator->() { return makePointerFromIterator(iterators); }

	ZipIterator& operator+=(const difference_type& rhs)
	{
		std::apply
		(
			[&](auto&... tupleArgs)
			{
				((tupleArgs += rhs), ...);
			}, iterators
		);
		return *this;
	}

	ZipIterator& operator-=(const difference_type& rhs)
	{
		std::apply
		(
			[&](auto&... tupleArgs)
			{
				((tupleArgs -= rhs), ...);
			}, iterators
		);
		return *this;
	}

	difference_type operator-(const ZipIterator& rhs)
	{
		return std::get<0>(this->iterators) - std::get<0>(rhs.iterators);
	}

	ZipIterator operator+(const difference_type& rhs)
	{
		ZipIterator tmp = *this;
		tmp += rhs;
		return std::move(tmp);
	}

	ZipIterator operator-(const difference_type& rhs)
	{
		ZipIterator tmp = *this;
		tmp -= rhs;
		return std::move(tmp);
	}
	
	// Prefix increment
	ZipIterator& operator++() {
		
		std::apply
		(
			[](auto&... tupleArgs)
			{
				(tupleArgs++,...);
			}, iterators
		);
		
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

		std::apply
		(
			[](auto&... tupleArgs)
			{
				(tupleArgs--, ...);
			}, iterators
		);

		return *this;
	}

	// Postfix increment
	ZipIterator operator--(int) {
		ZipIterator tmp = *this;
		this->operator--();
		return tmp;
	}

	reference operator[](const difference_type& rhs)
	{
		return *(*this + rhs);
	}

	bool operator>(const ZipIterator& rhs) const
	{
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
	
	friend bool operator== (const ZipIterator& a, const ZipIterator& b)
	{
		return a.iterators == b.iterators;
	}

	friend bool operator!= (const ZipIterator& a, const ZipIterator& b) {
		return std::get<0>(a.iterators) != std::get<0>(b.iterators);
	};

private:
	It iterators;

};

template<typename ...Cs>
struct MultiZipIterator {
	using type = ZipIterator<std::tuple<MultiIterator<Cs, Iterator<Cs>>...>,Cs...>;
};