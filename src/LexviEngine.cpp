#include "LexviEngine/pch.h"
#include "LexviEngine/LexviEngine.hpp"
#include "LexviEngine/Utils/GetSystemInfo.hpp"

namespace Lexvi {
	bool LexviEngine::Init() {
		m_app->Init();
		
		auto sysInfo = getSystemInfo();

		if (!sysInfo) {
			std::cout << "Failed to get system info: " << GetErrorString(sysInfo.error()) << std::endl;
			return false; // failed to init
		}

		std::cout << "OpenGL max version: " << sysInfo->glMajorVersion << "." << sysInfo->glMinorVersion << "\n";
		std::cout << "DSA"  << (!sysInfo->DSA_Support ? " NOT " : " ") << "supported" << std::endl;

		return true;
	}

	void LexviEngine::Shutdown() {
		m_app->Shutdown();
	}

	void LexviEngine::Run() {
		while (m_app->isRunning()) {
			std::cout << "Application running..." <<std::endl;
		}
	}
}
