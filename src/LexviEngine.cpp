#include "LexviEngine/pch.h"
#include "LexviEngine/LexviEngine.hpp"
#include "LexviEngine/Utils/GetSystemInfo.hpp"
#include "LexviEngine/Threading/ThreadRegistry.hpp"

namespace Lexvi {
	bool LexviEngine::Init() {	
		auto sysInfo = getSystemInfo();

		if (!sysInfo) {
			return false;
		}

		auto windowError = m_window.Init(WindowInfo {
				.size = {800, 600},
				.title = "Test",
				.VSYNC = true,
				.visible = true,
				.systemInfo = *sysInfo
				});

		if (windowError != WindowError::OK) {
			return false;
		}

		m_app->Init();

		return true;
	}

	void LexviEngine::Shutdown() {
		m_app->Shutdown();
	}

	void LexviEngine::Run() {
		auto err = ThreadRegistry::RegisterThread("Main");

		auto testThread = MakeNamedThread("Physics", []() {
				std::cout << "[" << ThreadRegistry::CurrentThreadName() << "] ";
				std::cout << "Physicsssss" << std::endl;
				return;
				});
		if (!testThread) {
			std::cout << "[Physcis]: " << GetErrorString(testThread.error());
		}

		using clock = std::chrono::steady_clock;
		auto lastFrameTime = clock::now();
		
		while (m_app->isRunning() && m_window.isOpen()) {
			auto currentFrameTime = clock::now();
			float delta_s = std::chrono::duration<float>(currentFrameTime - lastFrameTime).count();
			m_currentFrameTime += delta_s;
			lastFrameTime = currentFrameTime;

			m_window.ProcessCallbacks();

			glm::vec3 clearColor{0.0f};
			clearColor.r = glm::sin(m_currentFrameTime);
			clearColor.g = glm::cos(m_currentFrameTime);
			clearColor.b = glm::sin(m_currentFrameTime + glm::pi<float>());
			clearColor = (clearColor + glm::vec3(1.0f)) / 2.0f;

			glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

			m_window.SwapBuffers();
		}

		if(testThread) {
			std::thread& th = testThread.value();
			if (th.joinable()) th.join();
		}
	}
}
