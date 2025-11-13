#include "pch.h"

#include "Utils/Logging.hpp"

#include <string>

namespace {
	std::ofstream logFile;
}

namespace Lexvi {
	void RedirectLogToFile(const std::string& path) {
		logFile.open(path);
		if (!logFile.is_open()) {
			std::cerr << "Failed to open log file: " << path << std::endl;
		}
	}

	std::string getTimeStamp() {
		auto now = std::chrono::system_clock::now();
		auto timeT = std::chrono::system_clock::to_time_t(now);

		std::tm localTime{};
#ifdef _WIN32
		localtime_s(&localTime, &timeT);
#else
		localtime_r(&timeT, &localTime);
#endif

		std::ostringstream oss;
		oss << "[" << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << "]"
			<< " [T:" << std::this_thread::get_id() << "]";
		return oss.str();
	}

	void Log(const std::string& level, const std::string& message, const std::string& color)
	{
		std::ostringstream formatted;
		formatted << getTimeStamp() << " [" << level << "] " << message;

		// Console output (colorized)
		std::cout << color << formatted.str() << COLOR_RESET << std::endl;

		// File output (plain)
		if (logFile.is_open()) {
			logFile << formatted.str() << std::endl;
		}
	}
}