#include "LexviEngine/pch.hpp"
#include "LexviEngine/Renderer/Renderer.hpp"
#include "LexviEngine/Logging/Logging.hpp"

void Renderer::CopyBuffer(TransferBuffer&& src, const Buffer& dst) {
    m_transferBuffers.emplace_back( 
            std::make_pair(
                std::forward<TransferBuffer>(src), 
                vk::raii::Fence(m_device, vk::FenceCreateInfo{.flags = vk::FenceCreateFlagBits::eSignaled}))
        );

    vk::CommandBufferAllocateInfo allocInfo{.commandPool = m_commandPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = 1};
	vk::raii::CommandBuffer       commandCopyBuffer = std::move(m_device.allocateCommandBuffers(allocInfo).front());

	commandCopyBuffer.begin(vk::CommandBufferBeginInfo{.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
	commandCopyBuffer.copyBuffer(src.buffer, dst.buffer, vk::BufferCopy(0, 0, std::min(src.size, dst.size)));
	commandCopyBuffer.end();

	m_transferQueue.submit(vk::SubmitInfo{.commandBufferCount = 1, .pCommandBuffers = &*commandCopyBuffer}, m_transferBuffers.back().second);
}

void Renderer::SetVertexData(const VertexBuffer& buffer, const void* data, size_t size){
    TransferBuffer stagingBuffer = this->CreateBuffer<TransferBuffer>("Temp", size);
    
    {
        MappedDataRaw gpuData = *stagingBuffer.map();

        memcpy(gpuData.get(), data, size);
    }

    CopyBuffer(std::move(stagingBuffer), buffer);
}

void Renderer::UpdateVertexData(const VertexBuffer& buffer, size_t offset, const void* data, size_t size) const {

}
