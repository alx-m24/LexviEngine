#include "LexviEngine/pch.hpp"
#include "LexviEngine/Logging/Logging.hpp"
#include "LexviEngine/Renderer/Renderer.hpp"
#include "LexviEngine/Renderer/RenderGraph/RenderPass.hpp"
#include "LexviEngine/Renderer/RenderGraph/RenderGraph.hpp"
#include "LexviEngine/Renderer/RenderGraph/ImageResource.hpp"
#include "LexviEngine/Renderer/RenderContext/RenderContext.hpp"

static void TransitionImageLayout(vk::raii::CommandBuffer& commandBuffer, vk::Image image, [[maybe_unused]] vk::Format format,
    vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::ImageAspectFlags aspect) {
    vk::ImageMemoryBarrier barrier;

    barrier.setOldLayout(oldLayout)
            .setNewLayout(newLayout)
            .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
            .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
            .setImage(image)
            .setSubresourceRange({aspect, 0, 1, 0, 1});

    vk::PipelineStageFlags sourceStage;      // When previous operations must finish
    vk::PipelineStageFlags destinationStage; // When subsequent operations can start

    // Configure synchronization for undefined-to-transfer layout transitions
    // This pattern is common when preparing images for data uploads
    if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
        // Configure memory access permissions for upload preparation
        barrier.setSrcAccessMask(vk::AccessFlagBits::eNone)                // No previous access to synchronize
               .setDstAccessMask(vk::AccessFlagBits::eTransferWrite);      // Enable transfer write operations

        // Set pipeline stage synchronization points for upload workflow
        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;               // No previous work to wait for
        destinationStage = vk::PipelineStageFlagBits::eTransfer;           // Transfer operations can proceed

    } 
    // Configure synchronization for transfer-to-shader layout transitions
    // This pattern prepares uploaded images for shader sampling
    else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        // Configure memory access transition from writing to reading
        barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)       // Previous transfer writes must complete
               .setDstAccessMask(vk::AccessFlagBits::eShaderRead);         // Enable shader read access

        // Set pipeline stage synchronization for shader usage workflow
        sourceStage = vk::PipelineStageFlagBits::eTransfer;                // Transfer operations must complete
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;     // Fragment shaders can access
    }
    else if (oldLayout == vk::ImageLayout::eColorAttachmentOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        barrier.setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
                .setDstAccessMask(vk::AccessFlagBits::eShaderRead);

        sourceStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else if (oldLayout == vk::ImageLayout::eDepthAttachmentOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        barrier.setSrcAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentWrite)
               .setDstAccessMask(vk::AccessFlagBits::eShaderRead);

        sourceStage = vk::PipelineStageFlagBits::eLateFragmentTests;
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eColorAttachmentOptimal) {
        barrier.setSrcAccessMask({})
               .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    }
    else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthAttachmentOptimal) {
        barrier.setSrcAccessMask({})
               .setDstAccessMask(
                   vk::AccessFlagBits::eDepthStencilAttachmentRead |
                   vk::AccessFlagBits::eDepthStencilAttachmentWrite
               );

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
    }
    else if (oldLayout == vk::ImageLayout::eColorAttachmentOptimal && newLayout == vk::ImageLayout::ePresentSrcKHR) {
        barrier.setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
               .setDstAccessMask({}); // Present doesn't need access
                                      //

        sourceStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        destinationStage = vk::PipelineStageFlagBits::eBottomOfPipe;
    }
    else {
        LEXVI_ASSERT(false, "Unsupported image layout transition");
    }

    commandBuffer.pipelineBarrier(
        sourceStage,
        destinationStage,
        {},
        nullptr,
        nullptr, 
        barrier);
}

void transitionIfNeeded(vk::raii::CommandBuffer& cmd, ImageResource& res, bool isWrite) {
    vk::ImageLayout newLayout = res.pickLayout(isWrite ? ImageResource::ImageUsageIntent::WRITE : ImageResource::ImageUsageIntent::READ);

    if (res.currentLayout == newLayout)
        return;

    TransitionImageLayout(
        cmd,
        res.image,
        res.format,
        res.currentLayout,
        newLayout,
        (res.usage & vk::ImageUsageFlagBits::eDepthStencilAttachment)
            ? vk::ImageAspectFlagBits::eDepth
            : vk::ImageAspectFlagBits::eColor
    );

    res.currentLayout = newLayout;
}

void Renderer::Render() {
    std::shared_ptr<RenderGraph::RenderGraph> renderGraph = m_renderGraph.lock();
    if (!renderGraph) {
        return;
    }

    if (!renderGraph) {
        throw std::runtime_error("Render Graph not set: call Renderer::SetRenderGraph()");
    }

    auto fenceResult = m_device.waitForFences(*m_framesInFlightFence[frameIndex], vk::True, UINT64_MAX);
    if (fenceResult != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to wait for fence");
    }

    auto [result, imageIndex] = m_swapChain.acquireNextImage(UINT64_MAX, *m_presentCompleteSemaphores[frameIndex], nullptr);
	if (result == vk::Result::eErrorOutOfDateKHR) {
		reCreateSwapChain();
		return;
	}
    if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
		assert(result == vk::Result::eTimeout || result == vk::Result::eNotReady);
		throw std::runtime_error("failed to acquire swap chain image!");
	}

    {
        ImageResource backBuffer("BackBuffer");
        backBuffer.format = m_SwapChainSurfaceFormat.format;
        backBuffer.extent = m_swapChainExtent;
        backBuffer.usage = vk::ImageUsageFlagBits::eColorAttachment;
        backBuffer.currentLayout = vk::ImageLayout::eUndefined;
        backBuffer.image = m_swapChainImages[imageIndex]; // friend access
        backBuffer.view = m_swapChainImageViews[imageIndex]; // friend access
        renderGraph->SetResource("BackBuffer", std::move(backBuffer));
    }

    const RenderGraph::RenderGraph::OrderedNodes& nodes = renderGraph->getOrderedNodes()->get();

    vk::raii::CommandBuffer& buffer = m_commandBuffers[frameIndex];

    m_device.resetFences(*m_framesInFlightFence[frameIndex]);
    buffer.reset();
    buffer.begin({});

    RenderContext ctx(buffer);
        
    for (const std::unique_ptr<RenderGraph::RenderPass>& node : nodes) {
        for (auto& [name, img] : node->readImages) {
            transitionIfNeeded(buffer, *img, false);
        }
        
        for (auto& [name, img] : node->writeImages) {
            transitionIfNeeded(buffer, *img, true);
        }

        node->BeginPass(ctx);
        node->RunPass(ctx);
        node->EndPass(ctx);
    }
    
    // Submits to present command buffer
    TransitionImageLayout(buffer, m_swapChainImages[imageIndex], m_SwapChainSurfaceFormat.format, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR, vk::ImageAspectFlagBits::eColor);

    buffer.end();
    
    vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
	const vk::SubmitInfo submitInfo{
        .waitSemaphoreCount   = 1,
	    .pWaitSemaphores      = &*m_presentCompleteSemaphores[frameIndex], 
        .pWaitDstStageMask    = &waitDestinationStageMask, 
        .commandBufferCount   = 1,
        .pCommandBuffers      = &*m_commandBuffers[frameIndex], 
        .signalSemaphoreCount = 1,
        .pSignalSemaphores    = &*m_renderFinishedSemaphores[imageIndex]
    };
    m_graphicsQueue.submit(submitInfo, *m_framesInFlightFence[frameIndex]);
    
    vk::PresentInfoKHR presentInfoKHR {
        .waitSemaphoreCount = 1,
        .pWaitSemaphores    = &*m_renderFinishedSemaphores[imageIndex],
        .swapchainCount     = 1,
        .pSwapchains        = &*m_swapChain,
        .pImageIndices      = &imageIndex
    };
    result = m_presentQueue.presentKHR(presentInfoKHR);

    if ((result == vk::Result::eSuboptimalKHR) || (result == vk::Result::eErrorOutOfDateKHR) || m_frameBufferResized) {
        m_frameBufferResized = false;
    	reCreateSwapChain();
    }
    else {
    	assert(result == vk::Result::eSuccess);
    }


    for (auto it = m_transferBuffers.begin(); it != m_transferBuffers.end(); ) {
        if (it->second.getStatus() != vk::Result::eNotReady) {
            it = m_transferBuffers.erase(it); // destroys staging buffer and fence
        }
        else ++it;
    }

    frameIndex = (frameIndex + 1u) % MAX_FRAMES_IN_FLIGHT;
}
