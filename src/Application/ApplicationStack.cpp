#include "LexviEngine/pch.h"
#include "LexviEngine/Application/ApplicationStack.hpp"

namespace Lexvi {
	ApplicationStack::~ApplicationStack() {
		PopAll();
	}

	void ApplicationStack::AddApplicationLayer(std::unique_ptr<ApplicationLayer> app) {
		app->Init();

		m_Layers.push(std::move(app));
	}

	void ApplicationStack::PopApplicationLayer() {
		if (!m_Layers.empty()) {
			m_Layers.top()->Shutdown();
			m_Layers.pop();
		}

		if (!m_Layers.empty()) {
			m_Layers.top()->Reload();
		}
	}

	void ApplicationStack::UpdateTop()
	{
		if (!m_Layers.empty()) {
			m_Layers.top()->Update();
		}
	}

	void ApplicationStack::RenderTop()
	{
		if (!m_Layers.empty()) {
			m_Layers.top()->Render();
		}
	}

	void ApplicationStack::PopAll() {
		while (!m_Layers.empty()) {
			m_Layers.top()->Shutdown();
			m_Layers.pop();
		}
	}
}
