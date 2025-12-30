#pragma once

#include <vector>
#include <memory>

#include "ApplicationLayer.hpp"

namespace Lexvi {
	class ApplicationStack {
	    private:
	    	std::vector<std::unique_ptr<ApplicationLayer>> m_Layers;
            ApplicationLayer* m_currentLayer = nullptr;

        private:
            size_t m_pushTransitioning = 0;
            size_t m_popTransitioning = 0;

	    public:
	    	ApplicationStack() = default;
	    	~ApplicationStack();

	    	void AddApplicationLayer(std::unique_ptr<ApplicationLayer> app);
	    	void PopApplicationLayer();

	    	void UpdateActive();
	    	void RenderActive();

	    	void PopAll();
	};
}
