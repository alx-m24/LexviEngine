#pragma once

#include <optional>

namespace Lexvi {
	// Purely abstract base class for bindable resources
	class Bindable {
	public:
		virtual ~Bindable() = default;

	public:
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
	};


	// Scopped Binder
	class SmartBind {
	private:
		const Bindable& bindableResource;

	public:
		SmartBind(const Bindable& bindable) : bindableResource(bindable) {
			this->bindableResource.Bind();
		}

		~SmartBind() {
			this->bindableResource.Unbind();
		}
	};
};