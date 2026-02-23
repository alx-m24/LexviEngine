#pragma once

namespace RenderGraph { class RenderGraph; }

class ImageResource {
    friend class Renderer;
    friend class RenderContext;
    friend class RenderGraph::RenderGraph;
    friend void transitionIfNeeded(vk::raii::CommandBuffer& cmd, ImageResource& res, bool isWrite);

    private:
        std::string name = "";
        vk::Format format {};                    // Pixel format (RGBA8, Depth24Stencil8, etc.)
        vk::Extent2D extent {};                  // Dimensions in pixels for 2D resources
        vk::ImageUsageFlags usage {};            // How this resource will be used (color attachment, texture, etc.)
        vk::ImageLayout currentLayout {};        // Expected layout when the frame begins

        // Actual GPU resources - populated during compilation
        vk::Image image = nullptr;      // The GPU image object
        vk::raii::DeviceMemory memory = nullptr;  // Backing memory allocation
        vk::ImageView view = nullptr;   // Shader-accessible view of the image

    public:
        enum class ImageUsageIntent {
            WRITE,      // Writing as a color/depth/stencil attachment
            READ,       // Reading from a shader
            UPLOAD,     // Copying data from CPU/staging buffer
            TRANSFER    // Generic GPU-to-GPU transfer (copy)
        };

        vk::ImageLayout pickLayout(ImageUsageIntent intent) const {
            switch (intent) {
                case ImageUsageIntent::WRITE:
                    if (usage & vk::ImageUsageFlagBits::eDepthStencilAttachment)
                        return vk::ImageLayout::eDepthStencilAttachmentOptimal;
                    return vk::ImageLayout::eColorAttachmentOptimal;
        
                case ImageUsageIntent::READ:
                    if (usage & vk::ImageUsageFlagBits::eStorage)
                        return vk::ImageLayout::eGeneral;
                    return vk::ImageLayout::eShaderReadOnlyOptimal;
        
                case ImageUsageIntent::UPLOAD:
                    return vk::ImageLayout::eTransferDstOptimal;
        
                case ImageUsageIntent::TRANSFER:
                    // we can refine this depending on source/destination
                    return vk::ImageLayout::eGeneral; 
            }
        
            return vk::ImageLayout::eUndefined;
        }
    
    public:
        ImageResource() = default;
        ImageResource(const std::string& name) : name(name) {}

        bool operator==(const ImageResource& ir) const {
            return name == ir.name;
        }

        bool hasDepthComponent() const {
            switch (format)
            {
                case vk::Format::eD16Unorm:
                case vk::Format::eD32Sfloat:
                case vk::Format::eD24UnormS8Uint:
                case vk::Format::eD32SfloatS8Uint:
                    return true;
        
                default:
                    return false;
            }
        }
        
        bool hasStencilComponent() const {
            switch (format)
            {
                case vk::Format::eS8Uint:
                case vk::Format::eD24UnormS8Uint:
                case vk::Format::eD32SfloatS8Uint:
                    return true;
        
                default:
                    return false;
            }
        }
        
        bool isDepthOnlyImage() const {
            switch (format)
            {
                case vk::Format::eD16Unorm:
                case vk::Format::eD32Sfloat:
                    return true;
        
                default:
                    return false;
            }
        }
        
        bool isStencilOnlyImage() const {
            return format == vk::Format::eS8Uint;
        }
        
        bool isDepthStencilImage() const {
            switch (format)
            {
                case vk::Format::eD24UnormS8Uint:
                case vk::Format::eD32SfloatS8Uint:
                    return true;
        
                default:
                    return false;
            }
        }

    public:
        glm::uvec2 getExtent() const { return { extent.width, extent.height }; }
};
