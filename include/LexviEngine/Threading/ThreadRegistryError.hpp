#pragma once

#include <string_view>

namespace Lexvi {
	enum class ThreadRegistryError {
		OK = 0,
		THREAD_ALREADY_EXISTS,
		THREAD_NOT_EXISTS
	};

	[[nodiscard]] inline std::string_view GetErrorString(ThreadRegistryError error) {
		switch (error) {
			case ThreadRegistryError::THREAD_ALREADY_EXISTS:
				return "Thread already exists";
				break;
			case ThreadRegistryError::THREAD_NOT_EXISTS:
				return "Thread does not exists";
				break;
			default:
				return "No or unknown error";
				break;
		};
	}
}
