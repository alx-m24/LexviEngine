#pragma once

#include <memory>

#include "LexviEngine/Application/Application.hpp"
#include "LexviEngine/Window/Window.hpp"

namespace Lexvi {
	class LexviEngine {
		private:
			std::unique_ptr<Application> m_app;

		private:
			Window m_window;
			float m_currentFrameTime = 0.0f;

		public:
			LexviEngine(std::unique_ptr<Application> app) : m_app(std::move(app)) {}

		public:
			[[nodiscard]] bool Init();
			void Shutdown();
			void Run();
	};
}
