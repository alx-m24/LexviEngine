#pragma once

#include "ThreadRegistryError.hpp"

#include <expected>
#include <future>
#include <string_view>

namespace Lexvi {
	class ThreadRegistry {
		private:
			static thread_local std::string_view t_threadName;

		public:
			[[nodiscard]] static ThreadRegistryError RegisterThread(std::string_view name);
			[[nodiscard]] static ThreadRegistryError UnregisterThread();		
			[[nodiscard]] inline static std::string_view CurrentThreadName() { return t_threadName; }
	};

	template<typename Fn>
	inline std::expected<std::thread, ThreadRegistryError> MakeNamedThread(std::string_view name, Fn&& fn) {
		std::promise<ThreadRegistryError> readyPromise;
		auto fut = readyPromise.get_future();

		auto th = std::thread([name, f = std::forward<Fn>(fn), p = std::move(readyPromise)] () mutable {
				auto err = ThreadRegistry::RegisterThread(name);
				p.set_value(err);
				if (err != ThreadRegistryError::OK) return; // early exit from thread

				f(); // calling user function/callable

				err = ThreadRegistry::UnregisterThread(); // ignore this, we are sure thread exists
			});

		ThreadRegistryError regResult = fut.get(); // gets the registration results and blocks this thread while waiting
		if (regResult != ThreadRegistryError::OK) {
			return std::unexpected(regResult);
		}
		return std::move(th);
	}
}
