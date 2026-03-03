#include "LexviEngine/pch.hpp"
#include "LexviEngine/Renderer/Renderer.hpp"
#include "LexviEngine/Logging/Logging.hpp"

void Renderer::CopyBuffer(TransferBuffer&& src, const Buffer& dst) {
    vk::CommandBufferAllocateInfo allocInfo{.commandPool = m_commandPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = 1};
    m_transferBuffers.emplace_back( 
            std::make_pair(
                std::make_pair(std::forward<TransferBuffer>(src), std::move(m_device.allocateCommandBuffers(allocInfo).front())), 
                vk::raii::Fence(m_device, vk::FenceCreateInfo{ }))
        );
    TransferBuffer& srcBuffer = m_transferBuffers.back().first.first;
	vk::raii::CommandBuffer& commandCopyBuffer = m_transferBuffers.back().first.second;

	commandCopyBuffer.begin(vk::CommandBufferBeginInfo{.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
	commandCopyBuffer.copyBuffer(srcBuffer.buffer, dst.buffer, vk::BufferCopy(0, 0, std::min(srcBuffer.size, dst.size)));
	commandCopyBuffer.end();

	m_transferQueue.submit(vk::SubmitInfo{.commandBufferCount = 1, .pCommandBuffers = &*commandCopyBuffer}, m_transferBuffers.back().second);
}

void Renderer::SetVertexData(const VertexBuffer& buffer, const void* data, size_t size) {
    TransferBuffer stagingBuffer = this->CreateBuffer<TransferBuffer>(size);
    
    {
        MappedDataRaw gpuData = *stagingBuffer.map();

        memcpy(gpuData.get(), data, size);
    }

    CopyBuffer(std::move(stagingBuffer), buffer);
}

void Renderer::UpdateVertexData(const VertexBuffer& buffer, size_t offset, const void* data, size_t size) const {

}
