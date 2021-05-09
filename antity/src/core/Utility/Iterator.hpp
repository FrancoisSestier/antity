#pragma once
#include <vector>
#include <new>
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
	using iterator_category = std::forward_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using value_type = C;
	using pointer = C*;  // or also value_type*
	using reference = C&;  // or also value_type&

	Iterator(pointer ptr) : m_ptr(ptr) {}

	pointer get() { return m_ptr; }
	
	reference operator*() const { return *m_ptr; }
	pointer operator->() { return m_ptr; }

	// Prefix increment
	Iterator& operator++() { m_ptr++; return *this; }

	// Postfix increment
	Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

	friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
	friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; };

private:

	pointer m_ptr;
};


template<typename C,typename It>
struct MultiIterator
{
	using iterator_category = std::forward_iterator_tag;
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
			std::cout<<"here";
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

	

	
	friend bool operator== (const MultiIterator& a, const MultiIterator& b)
	{
		return (a.iterators.at(a.index) == b.iterators.at(a.index));
		/*
		if(a.iterators.size() != b.iterators.size())
		{
			return false;
		}
		for(size_t i = 0;i<a.iterators.size();i++)
		{
			if(a.iterators[i] != b.iterators[i])
			{
				return false;
			}
		};
		return true;
		*/
	};
	
	friend bool operator!= (const MultiIterator& a, const MultiIterator& b) {
		return (a.iterators.at(a.index) != b.iterators.at(a.index));
		/*
		if (a.iterators.size() != b.iterators.size())
		{
			return true;
		}
		for (size_t i = 0; i < a.iterators.size(); i++)
		{
			if (a.iterators[i] != b.iterators[i])
			{
				return true;
			}
		};
		return false;
		*/
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

	friend bool operator== (const ZipIterator& a, const ZipIterator& b)
	{
		return a.iterators == b.iterators;
	}


	friend bool operator!= (const ZipIterator& a, const ZipIterator& b) {
		return a.iterators != b.iterators;
	};

private:
	It iterators;

};

template<typename ...Cs>
struct MultiZipIterator {
	using type = ZipIterator<std::tuple<MultiIterator<Cs, Iterator<Cs>>...>,Cs...>;
};