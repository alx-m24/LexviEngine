#include "LexviEngine/pch.h"
#include "LexviEngine/Application/Application.hpp"

namespace Lexvi {
	bool Application::isRunning() const {
		return m_isRunning;
	}

	void Application::Update() {
		this->AppUpdate();
		m_AppStack.UpdateTop();
	}

	void Application::Render() {
		m_AppStack.RenderTop();
	}

	void Application::FullShutdown() {
		m_AppStack.PopAll();
		Shutdown();
	}
}
