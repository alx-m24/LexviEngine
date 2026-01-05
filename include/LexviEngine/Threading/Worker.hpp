#pragma once

#include <queue>
#include <functional>
#include <condition_variable>

#include "SmartThread.hpp"
#include "LexviEngine/ResourcePool/ResourcePool.hpp"

namespace Lexvi {
    namespace Thread {
        template<typename TResource, typename TInput>
        class Worker {
            public:
                using Handle = ResourcePool::ResourceHandle<TResource>;

            private:
                std::mutex queueWaitMutex;
                std::condition_variable m_queueCV;
                ResourcePool::ResourcePool<TResource>& m_pool;
                std::function<void(ResourcePool::Resource<TResource>&, const TInput&)> m_job;
                
                struct Item {
                    Handle handle;
                    TInput input;
                };
                std::queue<Item> m_queue;

                SmartThread m_thread; // declared last to destroy first
            public:
                Worker(std::string_view name, 
                        ResourcePool::ResourcePool<TResource>& pool,
                        std::function<void(ResourcePool::Resource<TResource>&, const TInput&)> job)
                    : m_pool(pool), 
                    m_job(job),
                    m_thread(name, [this](std::stop_token st) { this->ThreadLoop(st); }) {}

                [[nodiscard]] Handle Submit(const TInput& input) {
                    Handle handle = Emplace<TResource>(m_pool);

                    {
                        std::lock_guard<std::mutex> lock(queueWaitMutex);
                        m_queue.push( { handle, input } );
                    }

                    m_queueCV.notify_one();
                    return handle;
                }

                ~Worker() {
                    this->RequestStop();
                }

            private:
                void ThreadLoop(std::stop_token st) {
                    // Add lock free queue

                    while (!st.stop_requested()) {
                        Item item;

                        {
                            std::unique_lock<std::mutex> queueWaitLock(queueWaitMutex);
                            m_queueCV.wait(queueWaitLock,
                                    [this, &st]() { 
                                        return !m_queue.empty() || st.stop_requested();
                                    }
                                );

                            if (st.stop_requested()) break;

                            item = std::move(m_queue.front());
                            m_queue.pop();
                        }

                        ResourcePool::Resource<TResource>& resource = ResourcePool::Get(item.handle, m_pool);
                        m_job(resource, item.input);
                        resource.state.store(
                                ResourcePool::Resource<TResource>::State::Ready,
                                std::memory_order_release
                            );
                        }
                }

            public:
                void RequestStop() {
                    m_thread.RequestStop();
                    m_queueCV.notify_all();
                }
        };
    }
}
