#pragma once

#include <stack>
#include <memory>

#include "ApplicationLayer.hpp"

namespace Lexvi {
	class ApplicationStack {
	private:
		std::stack<std::unique_ptr<ApplicationLayer>> m_Layers;

	public:
		ApplicationStack() = default;
		~ApplicationStack();

		void AddApplicationLayer(std::unique_ptr<ApplicationLayer> app);
		void PopApplicationLayer();

		void UpdateTop();
		void RenderTop();

		void PopAll();
	};
}
