#pragma once

#include <string>

namespace Lexvi {
	void RedirectLogToFile(const std::string& path);
	std::string getTimeStamp();
	void Log(const std::string& level, const std::string& message, const std::string& color = "");
}

// Color escape codes for console (ignored by file output)
#define COLOR_RESET   "\033[0m"
#define COLOR_WARN    "\033[1;33m"  // Yellow
#define COLOR_ERROR   "\033[1;31m"  // Red

#define LEXVI_LOG_INFO(msg)  Lexvi::Log("INFO", msg, COLOR_RESET)
#define LEXVI_LOG_WARN(msg)  Lexvi::Log("WARN", msg, COLOR_WARN)
#define LEXVI_LOG_ERROR(msg) Lexvi::Log("ERROR", msg, COLOR_ERROR)