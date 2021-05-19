#pragma once
#include "Iterator.h"
#include <vector>
#include "VectorTuple.h"

namespace ant
{


	template<typename T>
	class VectorSequence
	{
		using underlying_iterator = typename std::vector<T>::iterator;
		using iterator = MultiIterator < T, underlying_iterator>;
		using underlying_type = T;
	public:
		VectorSequence(std::vector<std::vector<T>*> sequence)
		{
			for (auto&& vec : sequence)
			{
				begins.push_back(vec->begin());
				ends.push_back(vec->end());
			}
		}

		iterator begin() { return iterator{ begins, &begins, &ends }; }
		iterator end() { return iterator{ ends, &begins, &ends,ends.size()-1 }; }


	private:
		std::vector<underlying_iterator> begins;
		std::vector<underlying_iterator> ends;
	};

	template<typename ...Args>
	struct VectorZipper {
	
		using type = VectorZipper<Args...>;
		using underlying_iterator = std::tuple<MultiIterator<Args, typename  std::vector<Args>::iterator>...>;
		using iterator = typename  ZipIterator<std::tuple<MultiIterator<Args, typename  std::vector<Args>::iterator>...>, Args...>;
	
		VectorZipper(underlying_iterator begins, underlying_iterator ends) : begins(begins),ends(ends){}

		iterator begin() { return iterator{ begins }; }
		iterator end() { return iterator{ ends }; }

		underlying_iterator begins;
		underlying_iterator ends;
	};

	template<typename T, typename ...Args>
	VectorSequence<T> makeVectorSequence(Args&&... args)
	{
		return VectorSequence<T>(std::vector<std::vector<T>*>{&std::forward<Args>(args)...});
	}

	template<typename  ...Args>
	typename VectorZipper<Args...>::type zip(VectorSequence<Args>&... args)
	{
		return VectorZipper( std::make_tuple(args.begin()...), std::make_tuple(args.end()...) );
	}


	
}
