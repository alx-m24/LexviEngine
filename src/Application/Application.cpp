#include "LexviEngine/pch.h"
#include "LexviEngine/Application/Application.hpp"

namespace Lexvi {
	bool Application::isRunning() const {
		return m_isRunning;
	}

	void Application::update() {
		m_AppStack.UpdateTop();
	}

	void Application::render() {
		m_AppStack.RenderTop();
	}
}
