#include "LexviEngine/pch.hpp"
#include "LexviEngine/LexviEngine.hpp"

#include "LexviEngine/Time/Time.hpp"
#include "LexviEngine/Input/Input.hpp"
#include "LexviEngine/Logging/Logging.hpp"
#include "LexviEngine/Renderer/Renderer.hpp"
#include "LexviEngine/Renderer/RenderGraph/RenderGraph.hpp"
#include "LexviEngine/Threading/ThreadRegistry.hpp"

namespace Lexvi { 

    template<typename T>
    requires std::is_base_of_v<Application, T>
	[[nodiscard]] bool LexviEngine::Init(const std::string_view title) {
        Time::Init();

		[[maybe_unused]] auto err = Thread::RegisterThread("Main");
        
        m_renderer.Init(std::string(title));

        m_app = std::make_unique<T>(m_renderer);
        m_app->Init();

		return true;
	}

	void LexviEngine::Shutdown() {
		m_app->FullShutdown();
        m_renderer.Shutdown();
	}

	void LexviEngine::Run() {
		while (m_app->isRunning() && m_renderer.isRunning()) {
            Time::Update();

            m_renderer.Update();

            Input::CalculateDeltas();

			m_app->Update();

            Input::ClearFrameData();

            m_renderer.Render();
		}

    }
}
