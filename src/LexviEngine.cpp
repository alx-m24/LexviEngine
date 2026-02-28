#include "LexviEngine/pch.hpp"
#include "LexviEngine/LexviEngine.hpp"

#include "LexviEngine/Time/Time.hpp"
#include "LexviEngine/Input/Input.hpp"
#include "LexviEngine/Logging/Logging.hpp"
#include "LexviEngine/Renderer/Renderer.hpp"
#include "LexviEngine/Renderer/RenderGraph/RenderGraph.hpp"
#include "LexviEngine/Threading/ThreadRegistry.hpp"

namespace Lexvi { 


	void LexviEngine::Shutdown() {
		m_app->FullShutdown();
        m_app.reset();
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
