#pragma once
#include "../Utility/Iterator.hpp"
#include "Archetype.hpp"

namespace ant
{


	
	template<typename T>
	class View
	{
		using underlying_iterator = Iterator<T>;
		using iterator = MultiIterator < T, underlying_iterator>;
		using underlying_type = T;
	public:
		View() = default;
		
		View(std::vector<underlying_iterator>&& begins, std::vector<underlying_iterator>&& ends) :begins(begins),ends(ends)
		{
			
		}
		
		iterator begin() { return iterator{ begins, &begins, &ends }; }
		iterator end() { return iterator{ ends, &begins, &ends }; }


	private:
		std::vector<underlying_iterator> begins;
		std::vector<underlying_iterator> ends;
	};

	template<typename ...Args>
	class MultiView
	{
		using container = std::tuple<View<Args>...>;
		using underlying_iterator = std::tuple<MultiIterator<Args, Iterator<Args>>...>;
		using iterator = ZipIterator<std::tuple<MultiIterator<Args, Iterator<Args>>...>, Args...>;
	public:
		
		MultiView(container data) : data(data) {}

		iterator begin() { return iterator{ std::make_tuple(std::get<View<Args>>(data).begin()...) }; }
		iterator end() { return iterator{ std::make_tuple(std::get<View<Args>>(data).end()...) }; }

	private:
		container data;
	};

	template<typename ...Ts>
	struct view_tuple {
		using type = std::tuple<View<Ts>...>;
	};
	
	class ArchetypeViewBuilder {
	public:
		template<typename... Cs>
		MultiView<Entity, Cs...> BuildMultiComponentView(const std::vector<Archetype*>& archetypes) {
			auto entityView = std::make_tuple<View<Entity>>(BuildEntityView(archetypes));
			std::tuple<View<Cs>...> componentView = std::make_tuple(BuildComponentView<Cs>(archetypes)...);
			return MultiView<Entity, Cs...>(std::tuple_cat(std::move(entityView), std::move(componentView)));
		}

		template<typename T>
		View<T> BuildComponentView(std::vector<Archetype*> sequence)
		{
			std::vector<Iterator<T>> begins;
			std::vector<Iterator<T>> ends;
			ComponentTypeID componentType = TypeIdGenerator::GetTypeID<T>();
			for (auto&& archetype : sequence)
			{
				size_t index = std::ranges::find(archetype->archetypeId.begin(), archetype->archetypeId.end(), componentType) - archetype->archetypeId.begin();
				T* begin = reinterpret_cast<T*>(&archetype->componentArrays[index].componentData[0]);
				T* end = reinterpret_cast<T*>(&archetype->componentArrays[index].componentData[archetype->entities.size()* sizeof(T)]);
				begins.emplace_back(Iterator<T>(begin));
				ends.emplace_back(Iterator<T>(end));
			}
			return View<T>(std::move(begins), std::move(ends));
		}

		View<Entity> BuildEntityView(std::vector<Archetype*> archetypes)
		{
			std::vector<Iterator<Entity>> begins;
			std::vector<Iterator<Entity>> ends;

			for (auto&& archetype : archetypes)
			{
				begins.emplace_back(Iterator<Entity>(archetype->entities.cbegin()._Ptr));
				ends.emplace_back(Iterator<Entity>(archetype->entities.cend()._Ptr));
			}
			return View<Entity>(std::move(begins), std::move(ends));
		}
	};

}