#pragma once

#include <atomic>

namespace Lexvi {
    namespace ResourcePool {
        template<typename T>
        struct ResourceHandle {
            uint32_t id;
        };

        template<typename T>
        struct Resource {
            enum class State { Loading, Ready, Failed };
            std::atomic<State> state{State::Loading};
            T data;
        };

    }
}
