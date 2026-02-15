#include "LexviEngine/pch.hpp"
#include "LexviEngine/Application/Application.hpp"

namespace Lexvi {

	bool Application::isRunning() const {
		return m_isRunning;
	}

	void Application::Update() {
		this->AppUpdate();
		m_AppStack.UpdateActive();
	}

	void Application::Render() {
		m_AppStack.RenderActive();
	}

	void Application::FullShutdown() {
		m_AppStack.PopAll();
		Shutdown();
	}
}
