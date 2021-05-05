#pragma once
#include <algorithm>
#include <vector>
#include "Identifier.h"
#include "Component.h"
#include "../Utility/TypeList.h"
#include "../Utility/Hasher.h"
#include "../Utility/VectorTuple.h"

namespace ant
{
	using ArchetypeID = std::vector<ComponentTypeID>;

	template <typename... Cs>
	struct archetype_id
	{
		ArchetypeID value;

		archetype_id()
		{
			(value.push_back(Component<Cs>::GetTypeID()), ...);
			std::ranges::sort(value);
		}
	};

	template <typename... Cs>
	struct archetype_id<type_list<Cs...>> : archetype_id<Cs...>
	{
	};


	class ArchetypeBase
	{
	public:
		virtual ~ArchetypeBase()
		{
		};

		ArchetypeBase(ArchetypeBase&& archetypeBase) : archetypeID(std::move(archetypeBase.archetypeID)),
		                                               chunkID(chunkID)
		{
		}

		ArchetypeBase(ChunkID chunkId) : chunkID(chunkId)
		{
		}

		ArchetypeBase(const ArchetypeID& archetypeId, ChunkID chunkId)
			: archetypeID(archetypeId),
			  chunkID(chunkId)
		{
		}


		struct hasher
		{
			size_t operator()(const ArchetypeBase* key) const
			{
				return hash::hash_combine(hash::hash(key->archetypeID), hash::hash(key->chunkID));
			}
		};

		struct comparator
		{
			bool operator()(const ArchetypeBase* lhs, const ArchetypeBase* rhs) const
			{
				if (lhs->archetypeID.size() != rhs->archetypeID.size() || lhs->chunkID != rhs->chunkID)
				{
					return false;
				}

				for (size_t i = 0; i < lhs->archetypeID.size(); i++)
				{
					if (lhs->archetypeID[i] != rhs->archetypeID[i])
					{
						return false;
					}
				}

				return true;
			}
		};

	protected:
		ArchetypeID archetypeID;
		ChunkID chunkID;
	};

	template <typename ...Cs>
	class Archetype : public ArchetypeBase
	{
	public:
		~Archetype() override
		{
		}

		using type = type_list<Cs...>;
		using container_type = vector_tuple<Cs...>;

		Archetype(Archetype&& archetype) : componentData(std::move(archetype.componentData)),
		                                   entityIds(std::move(entityIds)), ArchetypeBase(std::move(archetype)) {}

        Archetype(ChunkID id = 0) : ArchetypeBase{ id }
        {
			(archetypeID.push_back(Component<Cs>::GetTypeID()),...);
            std::ranges::sort(archetypeID);
        }
		
    	template<typename C>
        std::vector<C>& GetComponents() {
            Assert<C>();
            return get<C>(componentData);
		}

        template<typename C>
        Component<C>::Iterator begin()
        {
            Assert<C>();
            return get<C>(componentData).begin();
        };

        template<typename C>
        Component<C>::Iterator end()
        {
            Assert<C>();
            return get<C>(componentData).end();
        };

        template<typename C>
        bool contains()
        {
            if (!contains_type<C, type::type>::value) {
                return false;
            }
            return true;
        }

        size_t size()
        {
            return std::tuple_size_v<type::type>;
        }
	
    private:
        template<typename C>
        void Assert()
        {
            if(!contains<C>()){
                throw std::runtime_error("Archertype Doesn't Contain requested Component");
            }
        }

        container_type componentData;

        std::vector<Entity> entityIds;
	};

	template <typename ...Cs>
	class Archetype<type_list<Cs...>> : public Archetype<Cs...>
	{
	public:
		Archetype(ChunkID chunkID = 0) : Archetype<Cs...>(chunkID)
		{
		};
	};

	template <typename ...Cs>
	class Archetype<std::tuple<Cs...>> : public Archetype<Cs...>
	{
	public:
		Archetype(ChunkID chunkID = 0) : Archetype<Cs...>(chunkID)
		{
		};
	};
}
