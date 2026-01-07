#pragma once

#include <queue>
#include <condition_variable>
#include "SmartThread.hpp"
#include "SystemThread.hpp"

namespace Lexvi {
    namespace Thread {
        template<typename TCommand>
        class CommandQueue {
            protected:
                std::queue<TCommand> m_commands;
                std::mutex m_queueMutex;

            public:
                void SubmitCommand(const TCommand& command) {
                        std::lock_guard<std::mutex> lock(m_queueMutex);
                        m_commands.push(command);
                }

            protected:
                void ProcessCommands() {
                    std::queue<TCommand> commandsCopy;

                    {
                        std::lock_guard<std::mutex> lock(m_queueMutex);
                        std::swap(commandsCopy, m_commands);
                    }

                    while (!commandsCopy.empty()) {
                        TCommand& command = commandsCopy.front();
                        ExecuteCommand(command);
                        commandsCopy.pop();
                    }
                }

                virtual void ExecuteCommand(const TCommand& cmd) = 0;
        };

        template<typename Derived, typename TBuffer, uint32_t RefreshRate, typename TCommand>
        requires std::copyable<TBuffer>
        class SystemThreadWithQueue : public CommandQueue<TCommand> {
            private:
                static_assert(RefreshRate > 0, "RefreshRate must be greater than 0");

            private:
                std::array<TBuffer, 2> m_buffers;
                std::atomic<bool> m_writeBuffer{false};
                
                SmartThread m_thread;
            
            public:
                SystemThreadWithQueue(std::string_view name)
                    : m_buffers{}, m_thread(name, [this](std::stop_token st) { ThreadLoop(st); }) {}
            
                ~SystemThreadWithQueue() { RequestStop(); }
            
            private:
                void ThreadLoop(std::stop_token st) {
                    Derived* derived = static_cast<Derived*>(this);

                    using namespace std::chrono;
                    constexpr auto interval = milliseconds(1000 / RefreshRate);
            
                    while (!st.stop_requested()) {
                        auto start = steady_clock::now();
            
                        // Process command queue
                        this->ProcessCommands();
            
                        derived->Tick();
            
                        // Flip buffers
                        bool current = m_writeBuffer.load(std::memory_order_relaxed);
                        m_writeBuffer.store(!current, std::memory_order_release);
            
                        std::this_thread::sleep_until(start + interval);
                    }
                }

            protected:
                TBuffer& getWriteBuffer() {
                    bool buffer = m_writeBuffer.load(std::memory_order_acquire);
                    return m_buffers[buffer];
                }
            
            public:
                TBuffer getLatest() const {
                    bool buffer = !m_writeBuffer.load(std::memory_order_acquire);
                    return m_buffers[buffer];
                }
            
                void RequestStop() {
                    m_thread.RequestStop();
                }
            };
    }
}
