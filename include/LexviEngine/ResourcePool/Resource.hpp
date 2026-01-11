#pragma once

#include <atomic>
#include <numbers>

namespace Lexvi {
    namespace ResourcePool {
        template<typename T>
        struct ResourceHandle {
            uint32_t id = 0;
            uint32_t generation = 0;

            bool operator==(const ResourceHandle<T>& h) const {
                return id == h.id && generation == h.generation;
            }

            static constexpr uint32_t InvalidId = std::numeric_limits<uint32_t>::max();
            static constexpr uint32_t InvalidGeneration = std::numeric_limits<uint32_t>::max();

            bool valid() const {
                return id != InvalidId && generation != InvalidGeneration;
            }
        };

        template<typename T>
        struct Resource {
            enum class State { Loading, Ready, Failed };
            std::atomic<State> state{State::Loading};
            std::atomic<uint32_t> generation{ 0 };
            T data;
        };

    }
}
