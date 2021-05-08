#pragma once

#include <unordered_map>
#include "Identifier.hpp"
#include "typeinfo"

namespace ant
{
	class RegistryDebugger
	{
	public:
		
		template<typename C>
		void OnRegisterComponent()
		{
			componentNameMap.emplace(TypeIdGenerator::GetTypeID<C>(), typeid(C).name() );
		}

	private:
		std::unordered_map<ComponentTypeID,std::string> componentNameMap ;
	};
}
