#pragma once

#include <string_view>

namespace Lexvi {
	enum class WindowError {
		OK = 0,
		GLFW_INIT_FAIL,
		GLFW_WINDOW_FAIL,
		GLAD_INIT_FAIL,
		DSA_NOT_SUPPORTED
	};

	[[nodiscard]] inline std::string_view GetErrorString(WindowError error) {
		switch (error) {
			case WindowError::GLFW_INIT_FAIL:
				return "Failed to init GLFW";
				break;
			case WindowError::GLFW_WINDOW_FAIL:
				return "Failed to create GLFW winodw";
				break;
			case WindowError::GLAD_INIT_FAIL:
				return "Failed to init GLAD";
				break;
			case WindowError::DSA_NOT_SUPPORTED:
				return "DSA not supported";
				break;
			default:
				return "No or unknown error";
				break;
		};
	}
}
