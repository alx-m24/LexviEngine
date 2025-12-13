#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "WindowError.hpp"
#include "LexviEngine/Utils/SystemInfo.hpp"

namespace Lexvi {
	using WindowSize = glm::uvec2;

	struct WindowInfo {
		WindowSize size;
		std::string_view title;
		bool VSYNC;
		bool visible;

		SystemInfo systemInfo;
	};

	class Window {
		private:
			GLFWwindow* m_window = nullptr;
			WindowInfo m_windowInfo;

		public:
			Window() = default;

			// RAII - GLFW context
			~Window();

		public:
			[[nodiscard]] WindowError Init(const WindowInfo& windowInfo);
			[[nodiscard]] WindowError Init(WindowInfo&& windowInfo);

		private:
			[[nodiscard]] WindowError CreateWindow();
			void DeleteWindow();

		public:
			[[nodiscard]] bool isOpen() const;
			void ProcessCallbacks();
			void SwapBuffers() const;
	};
}
