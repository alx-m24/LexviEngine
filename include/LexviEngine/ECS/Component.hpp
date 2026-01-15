#pragma once

namespace Lexvi {
    namespace ECS {
        struct ComponentTag {};

        template<typename T>
        concept Component = 
                std::derived_from<T, ComponentTag> &&
                std::is_standard_layout_v<T> &&
                std::is_trivially_copyable_v<T>;
    }
}
