#pragma once
#include <queue>
#include <concepts>

namespace ant {

	/// <summary>
	/// UniqueID Dispenser
	/// </summary>
	/// <typeparam name="T">must be unsigned</typeparam>
	template<std::unsigned_integral T >
	class UniqueIdDispenser {
	public:
		UniqueIdDispenser() = default;

		/// <summary>
		/// Ctor you can exclude some IDs by setting start id to a value > 0
		/// All IDs before start will never be used / given on set id exept if you explicitly free it using FreeID
		/// </summary>
		/// <returns></returns>
		UniqueIdDispenser(T start) {
			idCount = start;
			nextIDs.push(start);
		}

		/// <summary>
		/// Get a Unique ID 
		/// </summary>
		/// <returns> unsigned ID </returns>
		T GetID() {
			nextIDs.push(++idCount);
			auto id = nextIDs.front();
			nextIDs.pop();
			return id;
		}


		/// <summary>
		/// Free unused ID
		/// </summary>
		void FreeID(T id) {
			nextIDs.push(id);
		}

	private:
		std::queue<T> nextIDs;
		T idCount = 0;
	};




}
