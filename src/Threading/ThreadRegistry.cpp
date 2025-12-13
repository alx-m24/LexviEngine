#include "LexviEngine/pch.h"
#include "LexviEngine/Threading/ThreadRegistry.hpp"

namespace Lexvi {
	thread_local std::string_view ThreadRegistry::t_threadName = "Unknown";	
	
	static std::unordered_map<std::thread::id, std::string> g_registry;
	static std::mutex g_mutex;
	
	 ThreadRegistryError ThreadRegistry::RegisterThread(std::string_view name) {
		auto id = std::this_thread::get_id();

		std::lock_guard lock(g_mutex);

		if (g_registry.find(id) != g_registry.end()) {
			return ThreadRegistryError::THREAD_ALREADY_EXISTS;
		}

		g_registry[id] = name;
		t_threadName = g_registry[id];

		return ThreadRegistryError::OK;
	}

	ThreadRegistryError ThreadRegistry::UnregisterThread() {
		auto id = std::this_thread::get_id();

		std::lock_guard lock(g_mutex);


		if (g_registry.find(id) == g_registry.end()) {
			return ThreadRegistryError::THREAD_NOT_EXISTS;
		}

		t_threadName = "Unknown";
		g_registry.erase(id);

		return ThreadRegistryError::OK;
	}
}
