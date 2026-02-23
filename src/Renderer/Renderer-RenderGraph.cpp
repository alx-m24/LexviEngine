#include "LexviEngine/pch.hpp"
#include "LexviEngine/Logging/Logging.hpp"
#include "LexviEngine/Renderer/Renderer.hpp"
#include "LexviEngine/Renderer/RenderGraph/RenderGraph.hpp"
#include "LexviEngine/Renderer/Buffer/BufferDescription.hpp"
#include "LexviEngine/Renderer/Pipeline/Pipeline.hpp"
#include "LexviEngine/Renderer/Pipeline/PipelineDescription.hpp"

void Renderer::SetRenderGraph(std::shared_ptr<RenderGraph::RenderGraph> renderGraph) {
    m_renderGraph = renderGraph;
}
