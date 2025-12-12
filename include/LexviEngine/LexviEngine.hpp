#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>

#include "LexviEngine/Application/Application.hpp"

namespace Lexvi {
	class LexviEngine {
		private:
			std::unique_ptr<Application> m_app;

		private:
			GLFWwindow* window = nullptr;

		public:
			LexviEngine(std::unique_ptr<Application> app) : m_app(std::move(app)) {}

		public:
			[[nodiscard]] bool Init();
			void Shutdown();
			void Run();
	};
}
