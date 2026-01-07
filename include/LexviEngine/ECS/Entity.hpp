#pragma once

#include "LexviEngine/ResourcePool/ResourcePool.hpp"

namespace Lexvi {
    namespace ECS {
        struct EntityTag{};
        using Entity = ResourcePool::ResourceHandle<EntityTag>;
    }
}
