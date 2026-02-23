#pragma once

#include <memory>
#include <string_view>

#include "LexviEngine/Renderer/Renderer.hpp"
#include "LexviEngine/Application/Application.hpp"

namespace Lexvi {
	class LexviEngine {
		private:
			std::unique_ptr<Application> m_app;

		private:
            Renderer m_renderer;

		public:
			LexviEngine() = default;

		public:
            template<typename T>
            requires std::is_base_of_v<Application, T>
			[[nodiscard]] bool Init(const std::string_view title);

			void Shutdown();
			void Run();
	};
}
