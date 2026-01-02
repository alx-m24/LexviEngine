#pragma once

#include "ThreadRegistry.hpp"
#include "LexviEngine/Logging/Logging.hpp"
#include <thread>

namespace Lexvi {
    namespace Thread {
        template<typename Fn>
        void Invoke(Fn& fn, std::stop_token st) {
            if constexpr (std::invocable<Fn&, std::stop_token>) {
                    fn(st);
            }
            else {
                fn();
            }
        }

        class SmartThread {
            private:
                std::jthread m_thread;

            public:
                template<typename Fn>
                explicit SmartThread(std::string_view name, Fn&& fn) : m_thread([name, fn = std::forward<Fn>(fn)](std::stop_token st) mutable {
                        ThreadRegistryError err = RegisterThread(name);
                        if (err != ThreadRegistryError::OK) {
                            Log("Failed to create thread '{}': {}", name, GetErrorString(err));
                            return;
                        }
                      
                        auto fn_local = std::move(fn);
                        try {
                            Invoke<Fn>(fn_local, st);
                        }
                        catch (const std::exception& e) {
                            Log("Thread '{}' crashed: {}", name, e.what());
                        }
                        catch (...) {
                            Log("Thread '{}' crashed: Unknown Error", name);
                        }

                        err = UnregisterThread();
                        if (err != ThreadRegistryError::OK) {
                            Log("Failed to unregister thread '{}', {}", name, GetErrorString(err));
                        }  
                }) {}

                SmartThread(SmartThread&&) noexcept = default;
                SmartThread& operator=(SmartThread&&) noexcept = default;
                SmartThread(const SmartThread&) = delete;
                SmartThread& operator=(const SmartThread&) = delete;

            public:
                void RequestStop() { m_thread.request_stop(); }
        };
    }
}
