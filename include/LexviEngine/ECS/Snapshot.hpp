#pragma once

#include "ComponentStorage.hpp"

namespace Lexvi {
    namespace ECS {
        template<Component... Components>
        struct Snapshot {
            std::vector<Entity> entities;
            std::tuple<std::vector<Components>...> components;
        };
    }
}
