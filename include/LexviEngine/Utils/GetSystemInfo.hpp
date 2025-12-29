#pragma once

#include <expected>
#include <string_view>

#include "SystemInfo.hpp"

namespace Lexvi {
	enum class SystemInfoError {
		OK = 0,
		WINDOW_INIT_FAILED,
		GLAD_INIT_FAILED,
		VERSION_PARSE_FAILED
	};

	[[nodiscard]] inline std::string_view GetErrorString(SystemInfoError error) {
		switch (error) {
			case SystemInfoError::WINDOW_INIT_FAILED:
				return "Failed to init glfw window";
				break;
			case SystemInfoError::GLAD_INIT_FAILED:
				return "Failed to init glad and Opengl";
				break;
			case SystemInfoError::VERSION_PARSE_FAILED:
				return "Failed to get opengl version";
				break;
			default:
				return "No or unknown error";
				break;
		};
	}	

	[[nodiscard]] std::expected<SystemInfo, SystemInfoError> getSystemInfo() noexcept;
}
