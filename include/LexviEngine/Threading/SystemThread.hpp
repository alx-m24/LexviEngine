#pragma once

#include "SmartThread.hpp"

namespace Lexvi {
    namespace Thread {
        // Prefer using indirections (pointers, handles, references) inside TBuffer as getLatest() returns a copy
        template<typename Derived, typename TBuffer, uint32_t RefreshRate>
        requires std::copyable<TBuffer>
        class SystemThread {
            private:
                static_assert(RefreshRate > 0, "RefreshRate must be greater than 0");

            private:
                std::array<TBuffer, 2> m_buffers;
                std::atomic<bool> m_writeBuffer{false};

                SmartThread m_thread;

            public:
                SystemThread(std::string_view name) : 
                    m_thread(name, [this] (std::stop_token st) { ThreadLoop(st); }) {}

                ~SystemThread() { RequestStop(); }

            public:
                void ThreadLoop(std::stop_token st) {
                    Derived* derived = static_cast<Derived*>(this);

                    using namespace std::chrono;
                    constexpr auto interval = milliseconds(1000 / RefreshRate);

                    while (!st.stop_requested()) {
                        auto start = steady_clock::now();

                        derived->Tick();

                        // Flip write buffer
                        bool currentWrite = m_writeBuffer.load(std::memory_order_relaxed);
                        m_writeBuffer.store(!currentWrite, std::memory_order_release);

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
