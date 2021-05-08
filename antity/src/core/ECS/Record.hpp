#pragma once
#include "Archetype.hpp"

namespace ant {

	struct Record
	{
		Archetype* archetype;
		size_t index;
	};

	using EntityIndex = std::unordered_map < Entity, Record>;

}