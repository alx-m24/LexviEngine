#pragma once

#include "LexviEngine/Threading/ThreadRegistry.hpp"

#include <format>
#include <iostream>
#include <mutex>
#include <string_view>

namespace Lexvi {
	inline std::mutex g_logMutex;

	template<typename... Args>
	inline void Log(std::string_view fmt, Args&&... args) {
		using namespace Thread;
		std::string_view threadName = CurrentThreadName();

		std::lock_guard lock(g_logMutex);
		std::cout << "[" << threadName << "] "
		       	<< std::vformat(fmt, std::make_format_args(std::forward<Args>(args)...))
		       	<< "\n";
	}

	inline void Log(std::string_view msg) {
		using namespace Thread;
		std::string_view threadName = CurrentThreadName();

		std::lock_guard lock(g_logMutex);
		std::cout << "[" << threadName << "] " << msg << "\n";

	}
}

#ifndef NDEBUG

#define DEBUG_LOG(...)                                              \
    do {                                                            \
        Lexvi::Log(__VA_ARGS__);                                           \
    } while(false)

#define LEXVI_ASSERT(cond, ...)                                     \
    do {                                                            \
        if (!(cond)) {                                              \
            Lexvi::Log("Assertion Failed: {} ({}:{})", #cond, __FILE__, __LINE__);                     \
            Lexvi::Log(__VA_ARGS__);                                       \
            std::abort();                                           \
        }                                                           \
    } while (false)                                                 

#else
    
#define DEBUG_LOG(...) do { } while (false)
#define LEXVI_ASSERT(cond, ...) do { } while(false)

#endif
