#pragma once

#include "ThreadRegistryError.hpp"

#include <string_view>

namespace Lexvi {
	namespace Thread {
		[[nodiscard]] ThreadRegistryError RegisterThread(std::string_view name);
		[[nodiscard]] ThreadRegistryError UnregisterThread();		
		[[nodiscard]] std::string_view CurrentThreadName();
	};
}
