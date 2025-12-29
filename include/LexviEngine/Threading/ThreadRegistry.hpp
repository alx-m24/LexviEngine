#pragma once

#include "ThreadRegistryError.hpp"

#include <expected>
#include <future>
#include <string_view>

namespace Lexvi {
	namespace Thread {
		[[nodiscard]] ThreadRegistryError RegisterThread(std::string_view name);
		[[nodiscard]] ThreadRegistryError UnregisterThread();		
		[[nodiscard]] std::string_view CurrentThreadName();
	};

	template<typename Fn>
	inline std::expected<std::thread, Thread::ThreadRegistryError> MakeNamedThread(std::string_view name, Fn&& fn) {
		using namespace Thread;

		std::promise<ThreadRegistryError> readyPromise;
		auto fut = readyPromise.get_future();

		auto th = std::thread([name, f = std::forward<Fn>(fn), p = std::move(readyPromise)] () mutable {
				auto err = Thread::RegisterThread(name);
				p.set_value(err);
				if (err != ThreadRegistryError::OK) return; // early exit from thread

				f(); // calling user function/callable

				err = Thread::UnregisterThread(); // ignore this, we are sure thread exists
			});

		ThreadRegistryError regResult = fut.get(); // gets the registration results and blocks this thread while waiting
		if (regResult != ThreadRegistryError::OK) {
			return std::unexpected(regResult);
		}
		return std::move(th);
	}
}
