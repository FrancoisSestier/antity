#pragma once
#include <unordered_map>
#include "ArchetypeMap.h"
#include "../Utility/UniqueIdDispenser.h"

namespace ant {

	class Registry
	{
	private:
		typedef std::unordered_map<ComponentTypeID, ComponentBase*>
			ComponentTypeIDBaseMap;


	public:

		Entity CreateEntity()
		{
			Entity ent = entityIdDispenser.GetID();
			archetypeMap.OnEntityCreation(ent);
			return entityIdDispenser.GetID();
		}

		void RemoveEntity(Entity ent)
		{
			//TODO REMOVE COMPONENTS
			entityIdDispenser.FreeID(ent);
		}


	private:
		ComponentTypeIDBaseMap componentMap;
		ArchetypeMap archetypeMap;
		UniqueIdDispenser<Entity> entityIdDispenser;
	};

}
