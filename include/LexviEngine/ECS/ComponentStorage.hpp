#pragma once

#include "Entity.hpp"
#include "ECSErrors.hpp"
#include "Component.hpp"
#include "LexviEngine/Logging/Logging.hpp"
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

                std::vector<size_t> freeIndices;

            public:
                ComponentStorage() = default;
                ~ComponentStorage() = default;

                // NOT Copyable nor moveable
                ComponentStorage(const ComponentStorage&) = delete;
                ComponentStorage& operator=(const ComponentStorage&) = delete; 
                ComponentStorage(ComponentStorage&&) = delete;
                ComponentStorage& operator=(ComponentStorage&&) = delete; 

            public:
                std::vector<Entity>& getEntities() { return m_entities; };
                const std::vector<Entity>& getEntitiesConst() const { return m_entities; };

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

                    size_t mappedID; 
                    if (!freeIndices.empty()) {
                        mappedID = freeIndices.back();
                        freeIndices.pop_back();

                        m_entities[mappedID] = e;
                        m_storage[mappedID] = C(std::forward<Args>(args)...);
                    }
                    else {
                        mappedID = m_entities.size();
                        m_entities.push_back(e);
                        m_storage.emplace_back(std::forward<Args>(args)...);
                    }
                    
                    m_map[id] = mappedID;
                    
                    return AddError::OK;
                } 

                RemoveError Remove(const Entity& e) {
                    if (!e.valid()) return RemoveError::INVALID_ID;
                    if (!hasEntity(e)) return RemoveError::DOES_NOT_EXISTS;

                    // Changing m_entities[mappedID] should invalidate passed Entity
                    size_t id = static_cast<size_t>(e.id);
                    const size_t& mappedID = m_map[id];

                    m_entities[mappedID] = { Entity::InvalidId, Entity::InvalidGeneration };

                    freeIndices.push_back(mappedID);

                    return RemoveError::OK;
                }

                std::expected<std::reference_wrapper<C>, GetError> get(const Entity& e) {
                    if (!e.valid()) return std::unexpected(GetError::INVALID_ID);
                    if (!hasEntity(e)) return std::unexpected(GetError::DOES_NOT_EXISTS);

                    const size_t mappedID = m_map[static_cast<size_t>(e.id)];
                    return m_storage[mappedID];
                }

                std::expected<std::reference_wrapper<const C>, GetError> getConst(const Entity& e) const {
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

        template<Component T, Component... Types>
        constexpr const ComponentStorage<T>& GetComponentStorageConst(const ComponentStoragePool<Types...>& pool) {
            static_assert(
                    (std::is_same_v<T, Types> || ...),
                    "Requested component type does not exist in ComponentStoragePool"
            );

            return std::get<ComponentStorage<T>>(pool.pool); 
        }

    }
}
