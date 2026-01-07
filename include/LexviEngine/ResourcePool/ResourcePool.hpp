#pragma once

#include <tuple>
#include <type_traits>
#include <mutex>
#include "Resource.hpp"

namespace Lexvi {
    namespace ResourcePool {
        template<typename T>
        struct ResourcePool {
            std::atomic<uint32_t> nextIndex{0};
            std::vector<std::unique_ptr<Resource<T>>> pool;


            std::vector<uint32_t> freeIndices;
            std::mutex freeIndicesMutex;
            std::mutex emplaceMutex;
        };

        template<typename... Types>
        struct ResourcePools {
            std::tuple<ResourcePool<Types>...> pools;
        };

        template<typename T>
        ResourceHandle<T> Emplace(ResourcePool<T>& pool) {
            uint32_t index, generation;
            bool reused = false;
            {
                std::lock_guard<std::mutex> lock(pool.freeIndicesMutex);

                if (!pool.freeIndices.empty()) {
                    index = pool.freeIndices.back();
                    pool.freeIndices.pop_back();

                    generation = pool.pool[index]->generation.load(std::memory_order_relaxed);
                    reused = true;
                }
            }

            if (!reused) { // if we have not reused and index
                index = pool.nextIndex.fetch_add(1, std::memory_order_relaxed);
                generation = 0;

                {
                    std::lock_guard<std::mutex> lock(pool.emplaceMutex);
                    
                    if (index >= pool.pool.size()) {
                        pool.pool.resize(index + 1);
                    }

                    pool.pool[index] = std::make_unique<Resource<T>>();
                }
            }

            return ResourceHandle<T>{ index, generation };
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
        
        template<typename T>
        Resource<T>& Get(const ResourceHandle<T>& handle, const ResourcePool<T>& pool) {
            return *pool.pool.at(handle.id);
        }

        template<typename T, typename... Types>
        ResourcePool<T>& Get(const ResourceHandle<T>& handle, ResourcePools<Types...>& pools) {
            return Get(handle, GetPool<T>(pools));
        }

        template<typename T>
        bool isReady(const ResourceHandle<T>& handle, const ResourcePool<T>& pool) {
            return Get(handle, pool).state.load(std::memory_order_acquire) == Resource<T>::State::Ready; 
        }

        template<typename T>
        bool isValid(const ResourceHandle<T>& handle, const ResourcePool<T>& pool) {
            return Get(handle, pool).generation.load(std::memory_order_acquire) == handle.generation;
        }

        template<typename T>
        bool FreeResource(const ResourceHandle<T>& handle, ResourcePool<T>& pool) {
            if (!isValid(handle, pool)) return false;

            Resource<T>& res = Get(handle, pool);
            uint32_t oldGeneration = res.generation.load(std::memory_order_relaxed); 
            
            res.state.store(Resource<T>::State::Loading, std::memory_order_relaxed); 
            res.generation.store(oldGeneration + 1, std::memory_order_relaxed); 

            {
                std::lock_guard<std::mutex> lock(pool.freeIndicesMutex);
                pool.freeIndices.push_back(handle.id);
            }

            return true;
        }
    }
}
