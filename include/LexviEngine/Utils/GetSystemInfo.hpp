#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <expected>
#include <string>

namespace Lexvi {
	struct SystemInfo {
		uint32_t glMajorVersion;
		uint32_t glMinorVersion;
		bool DSA_Support;
	};

	enum class SystemInfoError {
		OK = 0,
		WINDOW_INIT_FAILED,
		GLAD_INIT_FAILED,
		VERSION_PARSE_FAILED
	};

	[[nodiscard]] inline std::string GetErrorString(SystemInfoError error) {
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

	[[nodiscard]] std::expected<SystemInfo, SystemInfoError> getSystemInfo() noexcept  {
		glfwInit();
		
		GLFWwindow* tempWindow = nullptr;
		for (int major = 4; major >= 3 && !tempWindow; --major) {
		    for (int minor = 6; minor >= 0 && !tempWindow; --minor) {
		        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
		        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
		        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		        tempWindow = glfwCreateWindow(10, 10, "Dummy", nullptr, nullptr);
		    }
		}
		if (!tempWindow) return std::unexpected(SystemInfoError::WINDOW_INIT_FAILED);
		
		glfwMakeContextCurrent(tempWindow);

		auto terminate = [&tempWindow]() {
			glfwDestroyWindow(tempWindow);
			glfwTerminate();
		};
	
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			terminate();
			return std::unexpected(SystemInfoError::GLAD_INIT_FAILED);
		}

		const GLubyte* version = glGetString(GL_VERSION); // e.g: 4.6.0 NVIDIA 558.3
		if (!version) {
			terminate();
        		return std::unexpected(SystemInfoError::VERSION_PARSE_FAILED);
		}
		
		const std::string versionString = reinterpret_cast<const char*>(version);
		unsigned int major = 0, minor = 0;
		if (sscanf(reinterpret_cast<const char*>(version), "%u.%u", &major, &minor) != 2) {
			terminate();
			return std::unexpected(SystemInfoError::VERSION_PARSE_FAILED);
		}

		bool coreDSA = (major > 4) || (major == 4 && minor >= 5);
		bool extDSA = glfwExtensionSupported("GL_EXT_direct_state_access");

		terminate();

		return SystemInfo {
			major,
			minor,
			coreDSA || extDSA
		};
	}
}
