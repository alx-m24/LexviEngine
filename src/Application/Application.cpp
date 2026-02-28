#include "LexviEngine/pch.hpp"
#include "LexviEngine/Renderer/Renderer.hpp"
#include "LexviEngine/Application/Application.hpp"

namespace Lexvi {

	bool Application::isRunning() const {
		return m_isRunning;
	}

	void Application::Update() {

		this->AppUpdate();

		m_AppStack.UpdateActive();
	}

	void Application::FullShutdown() {
        m_renderer.WaitIdle();

		m_AppStack.PopAll();
		Shutdown();
	}
}
