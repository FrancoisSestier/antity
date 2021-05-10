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

		virtual size_t GetSize() const = 0;
		virtual std::byte* Allocate(size_t size) = 0;
		virtual ComponentTypeID GetTypeID() = 0;
	};

	using ComponentMap = std::unordered_map<ComponentTypeID, std::unique_ptr<ComponentBase>>;

	
	template<class C>
	class Component : public ComponentBase
	{
	public:
		
		void DestroyData(std::byte* data) const override;
		void MoveData(std::byte* src, std::byte* dst) const override;
		void ConstructData(std::byte* data) const override;
		size_t GetSize() const override;
		ComponentTypeID GetTypeID() override;
		std::byte* Allocate(size_t size) override;
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
		return static_cast<ComponentTypeID>(TypeIdGenerator::GetTypeID<C>());
	}

	template <class C>
	std::byte* Component<C>::Allocate(size_t size)
	{
		alignas(C) std::byte* componentData = new std::byte[size];
		return componentData;
	}
}