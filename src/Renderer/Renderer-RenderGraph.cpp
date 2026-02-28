#include "LexviEngine/pch.hpp"
#include "LexviEngine/Renderer/Renderer.hpp"
#include "LexviEngine/Renderer/RenderGraph/RenderGraph.hpp"

void Renderer::SetRenderGraph(std::weak_ptr<RenderGraph::RenderGraph> renderGraph) {
    m_renderGraph = renderGraph;
    std::shared_ptr<RenderGraph::RenderGraph> tempShared = m_renderGraph.lock();
    tempShared->Compile(*this);
}
