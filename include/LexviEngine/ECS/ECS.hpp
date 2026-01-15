#pragma once

#include "Snapshot.hpp"
#include "ComponentStorage.hpp"
#include "LexviEngine/ResourcePool/ResourcePool.hpp"

#include <expected>

namespace Lexvi {
    namespace ECS {
        template<Component... Components>
        class ECS {
            private:
                using EntityPool = ResourcePool::ResourcePool<EntityTag>;

            private:
                EntityPool m_entities;
                ComponentStoragePool<Components...> m_pool;

                size_t m_entityCount = 0;

            public:
                ECS() = default;

            public:
                template<Component... EntityComponents>
                std::expected<Entity, AddError> CreateEntity() {
                    Entity temp_e = ResourcePool::Emplace(m_entities); // allocates a new entity in m_pool

                    AddError err = AddError::OK;
                    (
                        [&] {
                            if (err == AddError::OK) {
                                err = GetComponentStorage<EntityComponents>(m_pool).Add(temp_e);
                            }
                        } (),
                        ...
                    );

                    if (err != AddError::OK) {
                        DestroyEntity(temp_e); // immidiately deallocates if fail
                        return std::unexpected(err);
                    }

                    ++m_entityCount;
                    return temp_e;
                }

                void DestroyEntity(const Entity& e) {
                    if (!isAlive(e)) return;
                    
                    RemoveComponent<Components...>(e);
                    ResourcePool::FreeResource(e, m_entities);

                    --m_entityCount;
                }

                bool isAlive(const Entity& e) const {
                    return ResourcePool::isValid(e, m_entities);
                }

            public:
                template<Component... EntityComponents>
                AddError AddComponent(const Entity& e) {
                    AddError err = AddError::OK;
                    (
                        [&] {
                            if (err == AddError::OK) {
                                err = GetComponentStorage<EntityComponents>(m_pool).Add(e);
                            }
                        } (),
                        ...
                    );

                    return err;
                }

                template<Component... EntityComponents>
                RemoveError RemoveComponent(const Entity& e) {
                    RemoveError err = RemoveError::OK;

                    (
                        [&] {
                            if (err == RemoveError::OK) {
                                err = GetComponentStorage<EntityComponents>(m_pool).Remove(e);
                            }
                        } (),
                        ...
                    );

                    return err;
                }

                template<Component... EntityComponents>
                bool HasComponents(const Entity& e) const {
                    // HasComponents == Each ComponentStorage<EntityComponents...>::HasEntity()
                    return (GetComponentStorageConst<EntityComponents>(m_pool).hasEntity(e) && ...);
                }

                template<Component EntityComponent>
                std::expected<std::reference_wrapper<EntityComponent>, GetError> getSafeComponent(const Entity& e) {
                    return GetComponentStorage<EntityComponent>(m_pool).get(e);
                }

                template<Component EntityComponent>
               std::expected<std::reference_wrapper<const EntityComponent>, GetError> getSafeComponentConst(const Entity& e) const {
                    return GetComponentStorageConst<EntityComponent>(m_pool).getConst(e);
                }

                template<Component EntityComponent>
                EntityComponent& getComponent(const Entity& e) {
                    return (*GetComponentStorage<EntityComponent>(m_pool).get(e)).get();
                }

                template<Component EntityComponent>
                const EntityComponent& getComponentConst(const Entity& e) const {
                    return (*GetComponentStorageConst<EntityComponent>(m_pool).getConst(e)).get();
                }


            public:
                // Funs Fn on every entity having Components EntityComponents...
                template<Component First, Component... Others, typename Fn>
                void ForEach(Fn&& fn) {
                    ComponentStorage<First>& first = GetComponentStorage<First>(m_pool);

                    for (const Entity& e : first.getEntitiesConst()) {
                        if (!isAlive(e)) continue;
                        if (!HasComponents<Others...>(e)) continue;
                        
                        // Get references and call Fn
                        fn(e, *first.get(e), *GetComponentStorage<Others>(m_pool).get(e)...);
                    }
                }

                template<Component First, Component... Others, typename Fn>
                void ForEachConst(Fn&& fn) const {
                    const ComponentStorage<First>& first = GetComponentStorageConst<First>(m_pool);
                    
                    for (const Entity& e : first.getEntitiesConst()) {
                        // if (!e.valid()) continue;
                        if (!isAlive(e)) continue;
                        if (!HasComponents<Others...>(e)) continue;
                        
                        // Get references and call Fn
                        fn(e, *first.getConst(e), *GetComponentStorageConst<Others>(m_pool).getConst(e)...);
                    }
                }

            public:
                template<Component... EntityComponents>
                Snapshot<EntityComponents...> getSnapshot() const {
                    Snapshot<EntityComponents...> snapshot;

                    snapshot.entities.reserve(m_entityCount);
                    (..., std::get<std::vector<EntityComponents>>(snapshot.components).reserve(m_entityCount));

                    ForEachConst<EntityComponents...>(
                            [&snapshot] (const Entity& e, const EntityComponents&... components) {
                                snapshot.entities.push_back(e);
                                (..., std::get<std::vector<EntityComponents>>(snapshot.components).push_back(components));
                            }
                    );

                    return snapshot;
                }

                template<Component... EntityComponents>
                void getSnapshot(Snapshot<EntityComponents...>& snapshot) const {
                     snapshot.entities.clear();
                    (..., std::get<std::vector<EntityComponents>>(snapshot.components).clear());

                    snapshot.entities.reserve(m_entityCount);
                    (..., std::get<std::vector<EntityComponents>>(snapshot.components).reserve(m_entityCount));

                    ForEachConst<EntityComponents...>(
                            [&snapshot] (const Entity& e, const EntityComponents&... components) {
                                snapshot.entities.push_back(e);
                                (..., std::get<std::vector<EntityComponents>>(snapshot.components).push_back(components));
                            }
                    );
                }


            public:
                size_t getEntityCount() const { return m_entityCount; }
        };
    }
}
