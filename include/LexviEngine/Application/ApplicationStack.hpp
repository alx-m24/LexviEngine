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
            uint8_t m_pushTransitioning = 0;
            uint8_t m_popTransitioning = 0;

            std::vector<double> m_pushTransitionStart;
            std::vector<double> m_popTransitionStart;

	    public:
	    	ApplicationStack() = default;
	    	~ApplicationStack();

	    	void AddApplicationLayer(std::unique_ptr<ApplicationLayer> app);
	    	void PopApplicationLayer();

	    	void UpdateActive();

	    	void PopAll();
	};
}
