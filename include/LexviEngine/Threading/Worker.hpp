#pragma once

#include <functional>
#include <queue>

#include "SmartThread.hpp"
#include "LexviEngine/ResourcePool/ResourcePool.hpp"

namespace Lexvi {
    namespace Thread {
        template<typename TResource, typename TInput>
        class Worker {
            public:
                using Handle = ResourcePool::ResourceHandle<TResource>;

            private:
                SmartThread m_thread;
                ResourcePool::ResourcePool<TResource>& m_pool;
                std::function<void(ResourcePool::Resource<TResource>&, const TInput&)> m_job;
                
                struct Item {
                    Handle handle;
                    TInput input;
                };
                std::queue<Item> m_queue;

            public:
                Worker(std::string_view name, 
                        ResourcePool::ResourcePool<TResource>& pool,
                        std::function<void(ResourcePool::Resource<TResource>&, const TInput&)> job)
                    : m_pool(pool), 
                    m_job(job),
                    m_thread(name, [this](std::stop_token st) { this->ThreadLoop(st); }) {}

                Handle Submit(const TInput& input) {
                    Handle handle = Emplace<TResource>(m_pool);
                    m_queue.push( { handle, input } );
                    return handle;
                }

            private:
                void ThreadLoop(std::stop_token st) {
                    // Add conditional_variable
                    // Add lock free queue
                    while (!st.stop_requested()) {
                        Item item;

                        if (!m_queue.empty()) {
                            item = m_queue.front();
                            m_queue.pop();

                            ResourcePool::Resource<TResource>& resource = ResourcePool::Get(item.handle, m_pool);
                            m_job(resource, item.input);
                            resource.state.store(ResourcePool::Resource<TResource>::State::Ready, std::memory_order_release);
                        }
                    }
                }

            public:
                void RequestStop() {
                    m_thread.RequestStop();
                }
        };
    }
}
