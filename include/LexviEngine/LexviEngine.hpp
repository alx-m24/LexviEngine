#pragma once

#include <memory>
#include <string_view>

#include "LexviEngine/Time/Time.hpp"
#include "LexviEngine/Renderer/Renderer.hpp"
#include "LexviEngine/Application/Application.hpp"
#include "LexviEngine/Threading/ThreadRegistry.hpp"

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
	        [[nodiscard]] bool Init(const std::string_view title) {
                Time::Init();

	        	[[maybe_unused]] auto err = Thread::RegisterThread("Main");
                
                m_renderer.Init(std::string(title));

                m_app = std::make_unique<T>(m_renderer);
                m_app->Init();

	        	return true;
	        }

			void Shutdown();
			void Run();
	};
}
