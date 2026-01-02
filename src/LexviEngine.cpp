#include "LexviEngine/pch.h"
#include "LexviEngine/LexviEngine.hpp"
#include "LexviEngine/Input/Input.hpp"
#include "LexviEngine/Time/Time.hpp"
#include "LexviEngine/Logging/Logging.hpp"
#include "LexviEngine/Threading/SmartThread.hpp"
#include "LexviEngine/Utils/GetSystemInfo.hpp"
#include "LexviEngine/Threading/ThreadRegistry.hpp"

namespace Lexvi {
	bool LexviEngine::Init() {	
		auto err = Thread::RegisterThread("Main");

		auto sysInfo_expected = getSystemInfo();

		if (!sysInfo_expected) {
			Log("Failed to get system info");
			return false;
		}
		SystemInfo sysInfo = *sysInfo_expected;
		Log("OpenGL version: {}.{}", sysInfo.glMajorVersion, sysInfo.glMinorVersion);

		auto windowError = m_window.Init(WindowInfo {
				.size = {800, 600},
				.title = "Test",
				.VSYNC = true,
				.visible = true,
				.systemInfo = sysInfo
				});

		if (windowError != WindowError::OK) {
			Log("{}{}", "Failed to init window: ", GetErrorString(windowError));
			return false;
		}

        Time::Init();

		m_app->Init();

		return true;
	}

	void LexviEngine::Shutdown() {
		m_app->FullShutdown();
	}

	void LexviEngine::Run() {
        Thread::SmartThread worker("CounterThread", [](std::stop_token st) {
            int i = 0;
            while (!st.stop_requested()) {
                Log("Count: {}", ++i);
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
            Log("Stop requested, exiting...");
        });
        // worker.RequestStop();

		while (m_app->isRunning() && m_window.isOpen()) {
            Time::Update();

			m_window.ProcessCallbacks();
            Input::CalculateDeltas();

			m_app->Update();

            Input::ClearFrameData();

			m_app->Render();

			m_window.SwapBuffers();
		}
	}
}
