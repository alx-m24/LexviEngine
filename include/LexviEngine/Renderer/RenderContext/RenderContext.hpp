#pragma once

#include <vulkan/vulkan_raii.hpp>
#include <glm/glm.hpp>
#include <variant>
#include <span>

#include "LexviEngine/Renderer/RenderGraph/ImageResource.hpp"
#include "LexviEngine/Renderer/Pipeline/Pipeline.hpp"
#include "LexviEngine/Renderer/Buffer/Buffer.hpp"

enum class LoadOp {
    LOAD,
    CLEAR,
    DONT_CARE
};

struct DepthStencilClear {
    float depth = 1.0f;
    uint32_t stencil = 0;
};

using Color = glm::vec4;
using ClearVariant = std::variant<Color, DepthStencilClear>;

struct AttachmentDesc {
    ImageResource& image;
    LoadOp loadOp = LoadOp::LOAD;
    ClearVariant clearValue; 
};

class RenderContext {
    friend class Renderer;
    private:
        vk::raii::CommandBuffer& m_commandBuffer;

    public:
        RenderContext(vk::raii::CommandBuffer& commandBuffer) : m_commandBuffer(commandBuffer) {}

    public:
        void BeginRendering(std::span<const AttachmentDesc> attachments, glm::uvec2 renderExtent);

        void SetViewport(glm::uvec2 extent);
        void SetScissor(glm::uvec2 extent);

        void BindPipeline(const Pipeline& pipeline);
        
        void BindBuffer(const VertexBuffer& buffer);
        void BindBuffer(const IndexBuffer& buffer);

        void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);

        void EndRendering();
};
