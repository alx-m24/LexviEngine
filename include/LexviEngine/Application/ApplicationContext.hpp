#pragma once

namespace Lexvi {
	// Holds data for an application instance
	class ApplicationContext {
	public:
		ApplicationContext() = default;

		// Not copyable
		ApplicationContext(const ApplicationContext&) = delete;
		ApplicationContext& operator=(const ApplicationContext&) = delete;

		virtual ~ApplicationContext() = default;
	};

    template<typename T>
    concept ApplicationContext_T = std::is_base_of_v<ApplicationContext, T>;
}
