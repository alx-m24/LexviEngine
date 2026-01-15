#pragma once

#include <glm/glm.hpp>

#include "WindowError.hpp"
#include "LexviEngine/Utils/SystemInfo.hpp"

struct GLFWwindow;

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

        private:
            void setCallbacks();
            friend void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

		public:
			[[nodiscard]] bool isOpen() const;
			void ProcessCallbacks();
			void SwapBuffers() const;

        public:
            WindowSize getSize() const;
	};
}
