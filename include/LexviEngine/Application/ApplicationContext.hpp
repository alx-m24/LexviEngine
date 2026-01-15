#pragma once

namespace Lexvi {
	// Holds data for an application instance
	class ApplicationContext {
	public:
		ApplicationContext() = default;

		// Not copyable or movable
		ApplicationContext(const ApplicationContext&) = delete;
		ApplicationContext& operator=(const ApplicationContext&) = delete;

		ApplicationContext(ApplicationContext&&) = delete;
		ApplicationContext& operator=(ApplicationContext&&) = delete;


		virtual ~ApplicationContext() = default;

		virtual void Init() = 0;
		virtual void Shutdown() = 0;
	};
}