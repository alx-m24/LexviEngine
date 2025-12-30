#include "LexviEngine/pch.h"
#include "LexviEngine/Application/ApplicationStack.hpp"

namespace Lexvi {
	ApplicationStack::~ApplicationStack() {
		PopAll();
	}

	void ApplicationStack::AddApplicationLayer(std::unique_ptr<ApplicationLayer> app) {
		app->Init();

		m_Layers.push_back(std::move(app));
        m_pushTransitioning += 1;
	}

	void ApplicationStack::PopApplicationLayer() {
        m_popTransitioning += 1;
	}

	void ApplicationStack::UpdateActive()
	{
        if (m_Layers.empty()) return;

        if (m_currentLayer) m_currentLayer->Update();

        if (m_pushTransitioning) { // Run OnPush for top layer && update it 
            auto& top = m_Layers.back();
           
            bool doneTransitioning = top->OnPush(0.0f /*TODO*/);
            if (doneTransitioning) {
                m_pushTransitioning -= 1;
                m_currentLayer = top.get();
            }

            top->Update();
        }

        if (m_popTransitioning) { // Run OnPop for top layer && update layer right under
            auto& top = m_Layers.back();
            
            const size_t layerNum = m_Layers.size();
		    if (layerNum > 1) {
		    	m_Layers.at(layerNum - 2)->Update();
		    }

            bool doneTransitioning = top->OnPop(0.0f /*TODO*/);
            if (doneTransitioning) {
                if (m_popTransitioning > 0) m_popTransitioning -= 1;
                
                top->Shutdown();
                m_Layers.pop_back();

                if (!m_Layers.empty()) {
                    m_currentLayer = m_Layers.back().get();
                    m_currentLayer->Reload();
                }
            }
        }
	}

	void ApplicationStack::RenderActive()
	{
		if (m_Layers.empty()) return;

        if (m_currentLayer) m_currentLayer->Render();

        if (m_pushTransitioning || m_popTransitioning) {
            const size_t layerNum = m_Layers.size();
		    if (layerNum > 1) {
		    	m_Layers.at(layerNum - 2)->Render();
		    }
        }
	}

	void ApplicationStack::PopAll() {
        for (auto& layer : m_Layers) layer->Shutdown();
        m_Layers.clear();
	}
}
