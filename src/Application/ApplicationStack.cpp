#include "LexviEngine/pch.h"
#include "LexviEngine/Time/Time.hpp"
#include "LexviEngine/Logging/Logging.hpp"
#include "LexviEngine/Application/ApplicationStack.hpp"

namespace Lexvi {
	ApplicationStack::~ApplicationStack() {
		PopAll();
	}

	void ApplicationStack::AddApplicationLayer(std::unique_ptr<ApplicationLayer> app) {
		app->Init();

		m_Layers.push_back(std::move(app));
        m_pushTransitioning += 1;
        m_pushTransitionStart.push_back(Time::GetTime());
	}

	void ApplicationStack::PopApplicationLayer() {
        m_popTransitioning += 1;
        m_popTransitionStart.push_back(Time::GetTime());
        
        const size_t layerNum = m_Layers.size();
		if (layerNum > 1) {
            m_currentLayer = m_Layers.at(layerNum - 2).get();
            m_currentLayer->Reload();
		}
	}

	void ApplicationStack::UpdateActive()
	{
        if (m_Layers.empty()) return;

        if (m_currentLayer) m_currentLayer->Update();

        if (m_pushTransitioning) { // Run OnPush for top layer && update it 
            auto& top = m_Layers.back();
            
            double startTransitionTime = m_pushTransitionStart.back();
            auto transitionState = top->OnPush(static_cast<float>(Time::GetTime() - startTransitionTime));
            if (transitionState == ApplicationLayer::TransitionState::Completed) {
                if (m_pushTransitioning > 0) m_pushTransitioning -= 1;
                
                m_currentLayer = top.get();
                m_pushTransitionStart.pop_back();
            }

            top->Update();
        }
        if (m_popTransitioning) { // Run OnPop for top layer && update layer right under
            auto& top = m_Layers.back();

            double startTransitionTime = m_popTransitionStart.back();
            auto transitionState = top->OnPop(static_cast<float>(Time::GetTime() - startTransitionTime));
            if (transitionState == ApplicationLayer::TransitionState::Completed) {
                if (m_popTransitioning > 0) m_popTransitioning -= 1;
                
                top->Shutdown();
                m_Layers.pop_back();

                m_popTransitionStart.pop_back();

                    if (!m_Layers.empty()) {
                        m_currentLayer = m_Layers.back().get();
                    }
                    else {
                        m_currentLayer = nullptr;
                    }
            }
            else top->Update();
        }
	}

	void ApplicationStack::RenderActive()
	{
		if (m_Layers.empty()) return;

        if (m_currentLayer) m_currentLayer->Render();

        if (m_pushTransitioning || m_popTransitioning) {
            m_Layers.back()->Render();
        }
	}

	void ApplicationStack::PopAll() {
        for (auto& layer : m_Layers) layer->Shutdown();
        m_Layers.clear();
	}
}
