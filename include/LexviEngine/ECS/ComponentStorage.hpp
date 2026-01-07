#pragma once

#include "Entity.hpp"
#include "ECSErrors.hpp"
#include "Component.hpp"

#include <vector>
#include <expected>

namespace Lexvi {
    namespace ECS {
        template<Component C>
        class ComponentStorage {
            private:
                std::vector<C> m_storage;
                std::vector<Entity> m_entities;
                std::vector<size_t> m_map; // maps Entity::id to index in m_storage && m_entities

            public:
                ComponentStorage() = default;
                ~ComponentStorage() = default;

                // NOT Copyable nor moveable
                ComponentStorage(const ComponentStorage&) = delete;
                ComponentStorage& operator=(const ComponentStorage&) = delete; 
                ComponentStorage(ComponentStorage&&) = delete;
                ComponentStorage& operator=(ComponentStorage&&) = delete; 

            public:
                bool hasEntity(const Entity& e) const {
                    if (!e.valid()) return false;

                    size_t id = static_cast<size_t>(e.id);

                    if (id >= m_map.size()) return false;

                    const size_t& mappedID = m_map[id];

                    return mappedID < m_storage.size() && m_entities[mappedID] == e;
                }


                template<typename... Args>
                AddError Add(const Entity& e, Args... args) {
                    if (!e.valid()) return AddError::INVALID_ID;
                    if (hasEntity(e)) return AddError::ALREADY_EXISTS;

                    size_t id = static_cast<size_t>(e.id);

                    if (id >= m_map.size()) m_map.resize(id + 1);

                    const size_t mappedID = m_entities.size();
                    m_map[id] = mappedID;

                    m_entities.push_back(e);
                    m_storage.emplace_back(std::forward<Args>(args)...);
                    
                    return AddError::OK;
                } 

                std::expected<C&, GetError> get(const Entity& e) {
                    if (!e.valid()) return std::unexpected(GetError::INVALID_ID);
                    if (!hasEntity(e)) return std::unexpected(GetError::DOES_NOT_EXISTS);

                    const size_t mappedID = m_map[static_cast<size_t>(e.id)];
                    return m_storage[mappedID];
                }
        };

        template<Component... Components>
        struct ComponentStoragePool {
            std::tuple<ComponentStorage<Components>...> pool;
        };

        template<Component T, Component... Types>
        constexpr ComponentStorage<T>& GetComponentStorage(ComponentStoragePool<Types...>& pool) {
            static_assert(
                    (std::is_same_v<T, Types> || ...),
                    "Requested component type does not exist in ComponentStoragePool"
            );

            return std::get<ComponentStorage<T>>(pool.pool); 
        }
    }
}
