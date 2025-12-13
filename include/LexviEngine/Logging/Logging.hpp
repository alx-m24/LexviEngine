#pragma once

#include "LexviEngine/Threading/ThreadRegistry.hpp"

#include <format>
#include <mutex>

namespace Lexvi {
	inline std::mutex g_logMutex;

	template<typname... Args>
	inline void Log(std::string_view fmt, Args&&... args) {
		std::string_view threadName = ThreadRegistry::CurrentThreadName();

		std::lock_guard lock(g_logMutex);
		std::cout << "[" << threadName << "] " << std::vformat(fmt, std::make_format_args(args...)) << "\n";
	}
}
