#pragma once

#include <string>
#include <vector>

#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan_raii.hpp>

#include "LexviEngine/Renderer/Pipeline/PipelineDescription.hpp"
#include "LexviEngine/Renderer/Shader/Shader.hpp"
#include "LexviEngine/Renderer/Buffer/Buffer.hpp"

// Forward Declarations
class GLFWwindow;
class Pipeline;
namespace RenderGraph { 
    class RenderGraph;
}
namespace Extensions {
    struct Extension;
}

namespace DeviceExtensions {
    struct DeviceExtension {
        const char* name;

        bool operator==(const vk::ExtensionProperties& extensionProperties) const {
            return std::strcmp(name, extensionProperties.extensionName) == 0;
        }
    };

    const std::vector<DeviceExtension> requiredExtensions {
        DeviceExtension { vk::KHRSwapchainExtensionName }
    };
}

namespace Lexvi {
    class LexviEngine;
}

class Renderer {
    friend class Lexvi::LexviEngine;

    private:
        GLFWwindow* m_window = nullptr;

    private:
        vk::raii::Context m_context;

        vk::raii::Instance m_instance = VK_NULL_HANDLE;
        vk::raii::DebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;

        vk::raii::SurfaceKHR m_surface = VK_NULL_HANDLE;

        vk::raii::PhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
        vk::raii::Device m_device = VK_NULL_HANDLE;

        vk::raii::Queue m_presentQueue = VK_NULL_HANDLE;
        vk::raii::Queue m_graphicsQueue = VK_NULL_HANDLE;
        vk::raii::Queue m_transferQueue = VK_NULL_HANDLE;

        vk::raii::SwapchainKHR m_swapChain = VK_NULL_HANDLE;
        std::vector<vk::Image> m_swapChainImages;
        std::vector<vk::raii::ImageView> m_swapChainImageViews;

        vk::Extent2D m_swapChainExtent = {};
        vk::SurfaceFormatKHR m_SwapChainSurfaceFormat = {};

        vk::raii::CommandPool m_commandPool = VK_NULL_HANDLE;
        std::vector<vk::raii::CommandBuffer> m_commandBuffers;

        std::vector<vk::raii::Fence> m_framesInFlightFence;
        std::vector<vk::raii::Semaphore> m_presentCompleteSemaphores;
        std::vector<vk::raii::Semaphore> m_renderFinishedSemaphores;

        std::shared_ptr<RenderGraph::RenderGraph> m_renderGraph;

        VmaAllocator m_allocator{};
        
    private:
        uint32_t frameIndex = 0u;
        const uint32_t MAX_FRAMES_IN_FLIGHT = 3u;

    public:
        using QueueFamilyIndex = uint32_t;

    private:
        QueueFamilyIndex m_presentFamilyIndex;
        QueueFamilyIndex m_graphicsFamilyIndex;
        QueueFamilyIndex m_transferFamilyIndex;

    private:
        bool m_frameBufferResized = false;

    public:
        Renderer();
        ~Renderer();

    private:
        enum class InitResult : uint8_t {
            OK = 0,
            INSTANCE_FAILED,
            SURFACE_FAILED,
            PICK_PHYSICAL_DEVICE_FAILED,
            LOGICAL_DEVICE_FAILED,
            ALLOCATOR_FAILED
        };
        InitResult Init(const std::string& title);

        void Update();

        void Render();

        void Shutdown();

    public:
        bool isRunning() const;

    private:
        void InitGLFW(const std::string& title);

        static void FrameBufferSizeCallback(GLFWwindow *window, int width, int height); 

    private:
        void CreateInstance(const std::string& title);
        void SetupDebugMessenger();

    private:
        void CreateSurface();

    private:
        void PickPhysicalDevice();

    private:
        void CreateLogicalDeviceAndQueues();

    private:
        void CreateSwapChain();
        void reCreateSwapChain();
        void CleanupSwapChain();

    public:
        void SetRenderGraph(std::shared_ptr<RenderGraph::RenderGraph> renderGraph);

    private:
        void CreateCommandPool();
        void CreateCommandBuffers();

    private:
        void CreateSyncObjects();

    private:
        void CreateAllocator();

    private:
        std::vector<vk::PipelineShaderStageCreateInfo> getShaderStages(GraphicsShader& shader) const;
        vk::raii::PipelineLayout getPipelineLayout(PipelineDescription desc) const;
        void CreateVulkanPipeline(PipelineDescription desc, vk::raii::Pipeline& pipeline, vk::raii::PipelineLayout& layout) const;

    private:
        std::vector<Extensions::Extension> getRequiredExtensions() const;

    public:
        template<Buffer_T T, typename... Args>
        std::unique_ptr<T> CreateBuffer(Args&&... args) {
            std::unique_ptr<T> buffer = std::make_unique<T>(std::forward<Args>(args)...);
        
            vk::BufferCreateInfo bufferInfo {
                .size = buffer->size,
                .usage = buffer->usage,
            };
            
            VmaAllocationCreateInfo allocInfo{};
            allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        
            vmaCreateBuffer(m_allocator, &*bufferInfo, &allocInfo, &buffer->buffer, &buffer->allocation, nullptr);
            buffer->allocator = m_allocator;

            return std::move(buffer);
        }
        
        template<Buffer_T T>
        std::unique_ptr<T> CreateBuffer(const BufferDescription& desc) {
            return CreateBuffer<T>(desc.name, desc.size);
        }
};
