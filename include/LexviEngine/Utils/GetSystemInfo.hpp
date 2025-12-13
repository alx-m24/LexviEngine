#pragma once

#include <expected>
#include <string_view>

#include "LexviEngine/Window/Window.hpp"
#include "SystemInfo.hpp"

#include <glad/glad.h>

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

	[[nodiscard]] inline std::expected<SystemInfo, SystemInfoError> getSystemInfo() noexcept  {
		WindowInfo windowInfo {
			.size = {10, 10},
			.title = "",
			.VSYNC = false,
			.visible = false
		};
		Window tempWindow;

		bool windowCreated = false;	
		for (int major = 4; major >= 3 && !windowCreated; --major) {
		    for (int minor = 6; minor >= 0 && !windowCreated; --minor) {
			    windowInfo.systemInfo = {
				.glMajorVersion = static_cast<uint32_t>(major),
				.glMinorVersion = static_cast<uint32_t>(minor),
				.DSA_Support = true
			    };
			    windowCreated = (tempWindow.Init(windowInfo) == WindowError::OK);
		    }
		}
		if (!windowCreated) return std::unexpected(SystemInfoError::WINDOW_INIT_FAILED);

		const GLubyte* version = glGetString(GL_VERSION); // e.g: 4.6.0 NVIDIA 558.3
		if (!version) {
        		return std::unexpected(SystemInfoError::VERSION_PARSE_FAILED);
		}
		
		const std::string versionString = reinterpret_cast<const char*>(version);
		unsigned int major = 0, minor = 0;
		if (sscanf(reinterpret_cast<const char*>(version), "%u.%u", &major, &minor) != 2) {
			return std::unexpected(SystemInfoError::VERSION_PARSE_FAILED);
		}

		bool coreDSA = (major > 4) || (major == 4 && minor >= 5);
		bool extDSA = glfwExtensionSupported("GL_EXT_direct_state_access");

		return SystemInfo {
			major,
			minor,
			coreDSA || extDSA
		};
	}
}
