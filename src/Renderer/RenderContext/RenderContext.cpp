#include "LexviEngine/pch.hpp"
#include "LexviEngine/Logging/Logging.hpp"
#include "LexviEngine/Renderer/RenderContext/RenderContext.hpp"

void RenderContext::BeginRendering(std::span<const AttachmentDesc> attachments, glm::uvec2 renderExtent) {
    // NOTE: Renderer::Render() images have already been assigned currentLayout, thus it is safe to re-use it here

    std::vector<vk::RenderingAttachmentInfo> colorAttachments;
    colorAttachments.reserve(attachments.size());

    std::optional<vk::RenderingAttachmentInfo> depthAttachment;
    std::optional<vk::RenderingAttachmentInfo> stencilAttachment;

    for (const AttachmentDesc& desc : attachments) {
        vk::AttachmentLoadOp loadOp; 
        vk::ClearValue clearValue;

        switch (desc.loadOp) {
            case LoadOp::CLEAR:
                if (desc.image.hasDepthComponent()) {
                    LEXVI_ASSERT(std::holds_alternative<DepthStencilClear>(desc.clearValue), "Depth attachments cannot use color clear values");

                    DepthStencilClear depthStencilClear = std::get<DepthStencilClear>(desc.clearValue);
                    clearValue.setDepthStencil(vk::ClearDepthStencilValue(depthStencilClear.depth, depthStencilClear.stencil));
                }
                else {
                    LEXVI_ASSERT(std::holds_alternative<Color>(desc.clearValue), "Color attachments cannot use DepthStencil clear values");

                    glm::vec4 color = std::get<Color>(desc.clearValue);
                    clearValue.setColor(vk::ClearColorValue(color.r, color.g, color.b, color.a));
                }
                loadOp = vk::AttachmentLoadOp::eClear;
                break;
            case LoadOp::DONT_CARE:
                loadOp = vk::AttachmentLoadOp::eDontCare;
                break;
            case LoadOp::LOAD:
                loadOp = vk::AttachmentLoadOp::eLoad;
                break;
        }

        auto makeAttachmentInfo = [&](const ImageResource& img) -> vk::RenderingAttachmentInfo {
            return vk::RenderingAttachmentInfo{
                .imageView   = img.view,
                .imageLayout = img.currentLayout,
                .loadOp      = loadOp,
                .storeOp     = vk::AttachmentStoreOp::eStore,
                .clearValue  = clearValue
            };
        };

        if (desc.image.hasDepthComponent()) {
            depthAttachment.emplace(makeAttachmentInfo(desc.image));
        }
        if (desc.image.hasStencilComponent()) {
            stencilAttachment.emplace(makeAttachmentInfo(desc.image));
        }
        if (!desc.image.hasStencilComponent() && !desc.image.hasDepthComponent()) {
            colorAttachments.emplace_back(makeAttachmentInfo(desc.image));
        }
    }
    
    vk::RenderingInfo renderingInfo {
        .renderArea = { .offset = { 0, 0 }, .extent = { renderExtent.x, renderExtent.y } },
        .layerCount = 1,
        .colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size()),
        .pColorAttachments = colorAttachments.data(),
        .pDepthAttachment = (depthAttachment) ? &depthAttachment.value() : nullptr,
        .pStencilAttachment = (stencilAttachment) ? &stencilAttachment.value() : nullptr,
    };

    m_commandBuffer.beginRendering(renderingInfo);

    SetViewport(renderExtent);
    SetScissor(renderExtent);
}

void RenderContext::SetViewport(glm::uvec2 extent) {
    m_commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(extent.x), static_cast<float>(extent.y), 0.0f, 1.0f));
}

void RenderContext::SetScissor(glm::uvec2 extent) {
    m_commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), { extent.x, extent.y }));
}

void RenderContext::BindPipeline(const Pipeline& pipeline) {
    pipeline.Bind(m_commandBuffer);
}

void RenderContext::BindBuffer(const VertexBuffer& buffer) {
    m_commandBuffer.bindVertexBuffers(0, { buffer.buffer }, {0});
}

void RenderContext::BindBuffer(const IndexBuffer& buffer) {
    m_commandBuffer.bindIndexBuffer(buffer.buffer, 0, buffer.indexType);
}

void RenderContext::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
    m_commandBuffer.draw(vertexCount, instanceCount, firstInstance, firstInstance);
}

void RenderContext::EndRendering() {
    m_commandBuffer.endRendering();
}
