#pragma once
#include <iterator>
#include <new>
#include "Identifier.hpp"

namespace ant
{
	class ComponentBase
	{
	public:
		virtual ~ComponentBase() {}

		virtual void DestroyData(std::byte* data) const = 0;
		virtual void MoveData(std::byte* source, std::byte* destination) const = 0;
		virtual void ConstructData(std::byte* data) const = 0;

		virtual std::size_t GetSize() const = 0;
		virtual ComponentTypeID GetTypeID() = 0;
	};

	using ComponentMap = std::unordered_map<ComponentTypeID, std::unique_ptr<ComponentBase>>;

	
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
		
		void DestroyData(std::byte* data) const override;
		void MoveData(std::byte* src, std::byte* dst) const override;
		void ConstructData(std::byte* data) const override;
		size_t GetSize() const override;
		ComponentTypeID GetTypeID() override;
		
	};

	template<class C>
	void Component<C>::DestroyData(std::byte* data) const
	{
		C* dataLocation = std::launder(reinterpret_cast<C*>(data));

		dataLocation->~C();
	}

	template<class C>
	void Component<C>::ConstructData(std::byte* data) const
	{
		new (&data[0]) C();
	}

	template<class C>
	void Component<C>::MoveData(std::byte* source, std::byte* destination) const
	{
		new (&destination[0]) C(std::move(*reinterpret_cast<C*>(source)));
	}

	template<class C>
	std::size_t Component<C>::GetSize() const
	{
		return sizeof(C);
	}

	template <class C>
	ComponentTypeID Component<C>::GetTypeID()
	{
		return TypeIdGenerator::GetTypeID<C>();
	}
	
}