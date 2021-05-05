#pragma once
#include <iterator>
#include <new>
#include "Identifier.h"

namespace ant
{
	class ComponentBase
	{
	public:
		virtual ~ComponentBase() {}

		virtual void DestroyData(unsigned char* data) const = 0;
		virtual void MoveData(unsigned char* source, unsigned char* destination) const = 0;
		virtual void ConstructData(unsigned char* data) const = 0;

		[[nodiscard]] virtual std::size_t GetSize() const = 0;
	};

	
	template<class C>
	class Component : public ComponentBase
	{
	public:

		struct Iterator
		{
			using iterator_category = std::forward_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = C;
			using pointer = C*;  // or also value_type*
			using reference = C&;  // or also value_type&
			
			Iterator(pointer ptr) : m_ptr(ptr) {}


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
		
		void DestroyData(C* data) const override;
		void MoveData(C* src, C* dst) const override;
		void ConstructData(C* data) const override;
		static size_t GetSize(C* data);
		static ComponentTypeID GetTypeID();
		
	};

	template <class C>
	void Component<C>::DestroyData(C* data) const
	{
		std::launder(data)->~C();
	}

	template <class C>
	void Component<C>::MoveData(C* src, C* dst) const
	{
		new (dst) C(std::move(*src));
	}

	template <class C>
	void Component<C>::ConstructData(C* data) const
	{
		new (data) C();
	}

	template <class C>
	size_t Component<C>::GetSize(C* data)
	{
		return sizeof(C);
	}

	template <class C>
	ComponentTypeID Component<C>::GetTypeID()
	{
		return TypeIdGenerator<ComponentTypeID>::GetNewID<C>();
	}
	
}