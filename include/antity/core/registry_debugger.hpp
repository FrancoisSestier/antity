#pragma once

#include <antity/core/identifier.hpp>
#include <unordered_map>

#include "typeinfo"

namespace ant {
    class registry_debugger {
       public:
        template <typename C>
        void OnComponentRegistration() {
            componentNameMap.emplace(type_id_generator::get<C>(),
                                     typeid(C).name());
        }

       private:
        std::unordered_map<component_id_t, std::string> componentNameMap;
    };
}  // namespace ant
