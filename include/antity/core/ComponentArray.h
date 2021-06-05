#include "Archetype.h"

namespace ant {

    template<typename T>
    struct ComponentArray{
        using iterator = Iterator<T>;

        ComponentArray() = default;

        ComponentArray(ByteArray* byteArray) : 
                data(reinterpret_cast<T*>(byteArray->componentData)),
                byteArray(byteArray) {}

        [[nodiscard]] const T& operator[](size_t index) const {
			return data[index];
		}

		[[nodiscard]] T& operator[](size_t index) {
			return data[index];
		}

		bool valid() const noexcept{
			return data != nullptr;
		}

		T* begin() {
			return data;
		}

		T* end() {
			return data + byteArray->size;
		}

		int16_t size() {
			return byteArray->size;
		}

		T* data { nullptr };
        ByteArray* byteArray {nullptr};
    };

    template<typename T>
    auto GetComponentArray(Archetype* archetype){
        auto typeId = TypeIdGenerator::GetTypeID<T>();

        for(int i = 0; i < archetype->archetypeId.size() ;i++){
            if(typeId == archetype->archetypeId[i]){
                return(ComponentArray<std::remove_reference_t<T>>(&archetype->byteArrays[i]));
            }
        }

        return ComponentArray<std::remove_reference_t<T>>();
    }

}