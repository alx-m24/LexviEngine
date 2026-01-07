#pragma once

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

                    return temp_e;
                }

                void DestroyEntity(const Entity& e) {
                    ResourcePool::FreeResource(e, m_entities);
                }

                bool isAlive(const Entity& e) const {
                    return ResourcePool::isValid(e, m_entities);
                }

            public:
                template<Component... EntityComponents>
                AddError AddComponent(const Entity& e) {
                    return AddError::OK;
                }
        };
    }
}
