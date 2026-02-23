#pragma once 

#include <vma/vk_mem_alloc.h>

#include "BufferDescription.hpp"

class Buffer {
    friend class Renderer;

    protected:
        std::string name = "";
        VkBuffer buffer = VK_NULL_HANDLE;
        VmaAllocation allocation = {};
        vk::BufferUsageFlags usage = {};
        
        VmaAllocator allocator = VK_NULL_HANDLE;

    public:
        size_t size = 0;

    public:
        Buffer(const std::string& name) : name(name) {}

        Buffer(const std::string& name, size_t size) : name(name), size(size) {}

        Buffer(const std::string& name, BufferUsage bufferUsage) : name(name), usage(getUsageFlags(bufferUsage)) {}
        Buffer(const std::string& name, size_t size, BufferUsage bufferUsage) : name(name), usage(getUsageFlags(bufferUsage)), size(size) {}

        ~Buffer() {
            if (buffer) {
                vmaDestroyBuffer(allocator, buffer, allocation);
                buffer = VK_NULL_HANDLE;
            }
        }

        Buffer(const Buffer&) = delete;
        void operator=(const Buffer&) = delete;

        Buffer(Buffer&& other) {
            this->name = std::move(other.name);
            this->buffer = other.buffer;
            this->allocation = other.allocation;
            this->usage = other.usage;
            this->allocator = other.allocator;

            other.buffer = VK_NULL_HANDLE;
            other.name = "";
            other.allocation = {};
            other.usage = {};
            other.allocator = VK_NULL_HANDLE;
        }
        Buffer& operator=(Buffer&& other) {
            if (this != &other) {
                if (buffer) {
                    vmaDestroyBuffer(allocator, buffer, allocation);
                }

                this->name = std::move(other.name);
                this->buffer = other.buffer;
                this->allocation = other.allocation;
                this->usage = other.usage;
                this->allocator = other.allocator;

                other.buffer = VK_NULL_HANDLE;
                other.name = "";
                other.allocation = {};
                other.usage = {};
            }

            return *this;
        }

    private:
        static vk::BufferUsageFlags getUsageFlags(BufferUsage usage);

    public:
        std::string getName() const { return name; }

        bool operator==(const Buffer& other) const {
            return name == other.name && size == other.size && usage == other.usage;
        }
        bool operator==(const BufferDescription& desc) const {
            return name == desc.name && size == desc.size && usage == getUsageFlags(desc.usage);
        }
};

template<typename T>
concept Buffer_T = std::is_base_of_v<Buffer, T>;

class VertexBuffer : public Buffer {
    public:
        VertexBuffer(const std::string& name) : Buffer(name, BufferUsage::VERTEX_BUFFER) {}
        VertexBuffer(const std::string& name, size_t size) : Buffer(name, size, BufferUsage::VERTEX_BUFFER) {}
};

class UniformBuffer : public Buffer {
    public:
        UniformBuffer(const std::string& name) : Buffer(name, BufferUsage::UNIFORM_BUFFER) {}
        UniformBuffer(const std::string& name, size_t size) : Buffer(name, size, BufferUsage::UNIFORM_BUFFER) {}
};

class TransferBuffer : public Buffer {
    public:
        TransferBuffer(const std::string& name) : Buffer(name, BufferUsage::TRANSFER_BUFFER) {}
        TransferBuffer(const std::string& name, size_t size) : Buffer(name, size, BufferUsage::TRANSFER_BUFFER) {}
};
