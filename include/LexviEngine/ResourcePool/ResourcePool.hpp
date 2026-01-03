#pragma once

#include <tuple>
#include <type_traits>
#include "Resource.hpp"

namespace Lexvi {
    namespace ResourcePool {
        template<typename T>
        struct ResourcePool {
            std::atomic<uint32_t> nextIndex{0};
            std::vector<std::unique_ptr<Resource<T>>> pool;
        };

        template<typename... Types>
        struct ResourcePools {
            std::tuple<ResourcePool<Types>...> pools;
        };

        template<typename T>
        ResourceHandle<T> Emplace(ResourcePool<T>& pool) {
            uint32_t index = pool.nextIndex.fetch_add(1, std::memory_order_relaxed);
            pool.pool.emplace_back(std::make_unique<Resource<T>>());

            return ResourceHandle<T>{ index };
        }

        template<typename T, typename... Types>
        auto& GetPool(ResourcePools<Types...>& pools) {
            static_assert(
                    (std::is_same_v<T, Types> || ...),
                    "Requested resource type does not exist in ResourcePools"
            );

            return std::get<ResourcePool<T>>(pools.pools); 
        }

        template<typename T>
        Resource<T>& Get(const ResourceHandle<T>& handle, ResourcePool<T>& pool) {
            return *pool.pool.at(handle.id);
        }

        template<typename T, typename... Types>
        ResourcePool<T>& Get(const ResourceHandle<T>& handle, ResourcePools<Types...> pools) {
            return Get(handle, GetPool<int>(pools));
        }

        template<typename T>
        bool isReady(const ResourceHandle<T>& handle, ResourcePool<T>& pool) {
            return Get(handle, pool).state.load() == Resource<T>::State::Ready; 
        }
    }
}
