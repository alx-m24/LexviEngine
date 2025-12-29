#pragma once

#include "LexviEngine/Threading/ThreadRegistry.hpp"

#include <format>
#include <iostream>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>

namespace Lexvi {
	inline std::mutex g_logMutex;

	template<typename... Args>
	inline void Log(std::string_view fmt, Args&&... args) {
		using namespace Thread;
		std::string_view threadName = CurrentThreadName();

		std::lock_guard lock(g_logMutex);
		std::cout << "[" << threadName << "] "
		       	<< std::vformat(fmt, std::make_format_args(args...))
		       	<< "\n";
	}

	inline void Log(std::string_view msg) {
		using namespace Thread;
		std::string_view threadName = CurrentThreadName();

		std::lock_guard lock(g_logMutex);
		std::cout << "[" << threadName << "] " << msg << "\n";
	}
}
