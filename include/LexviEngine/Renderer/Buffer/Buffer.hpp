#pragma once 

#include <vk_mem_alloc.h>

#include "LexviEngine/Logging/Logging.hpp"
#include "BufferDescription.hpp"

#include <expected>

template<typename T>
class MappedData;

class Buffer {
    friend class Renderer;
    friend class RenderContext;
    template<typename T>
    friend class MappedData;

    protected:
        VkBuffer buffer = VK_NULL_HANDLE;
        VmaAllocation allocation = {};
        vk::BufferUsageFlags usage = {};
        
        VmaAllocator allocator = VK_NULL_HANDLE;

    public:
        size_t size = 0;

    public:
        Buffer() = default;

        Buffer(size_t size) : size(size) {}

        Buffer(BufferUsage bufferUsage) : usage(getUsageFlags(bufferUsage)) {}
        Buffer(size_t size, BufferUsage bufferUsage) : usage(getUsageFlags(bufferUsage)), size(size) {}

        ~Buffer() {
            if (buffer) {
                LEXVI_ASSERT(allocator != VK_NULL_HANDLE, "Allocator is NULL");
                vmaDestroyBuffer(allocator, buffer, allocation);
                buffer = VK_NULL_HANDLE;
            }
        }

        Buffer(const Buffer&) = delete;
        void operator=(const Buffer&) = delete;

        Buffer(Buffer&& other) noexcept {
            if (this != &other) {
                if (buffer) {
                    vmaDestroyBuffer(allocator, buffer, allocation);
                }

                this->buffer = other.buffer;
                this->allocation = other.allocation;
                this->usage = other.usage;
                this->allocator = other.allocator;
                this->size = other.size;

                other.buffer = VK_NULL_HANDLE;
                other.allocation = {};
                other.usage = {};
                other.allocator = VK_NULL_HANDLE;
                other.size = 0;
            }
        }
        Buffer& operator=(Buffer&& other) noexcept {
            if (this != &other) {
                if (buffer) {
                    vmaDestroyBuffer(allocator, buffer, allocation);
                }

                this->buffer = other.buffer;
                this->allocation = other.allocation;
                this->usage = other.usage;
                this->allocator = other.allocator;
                this->size = other.size;

                other.buffer = VK_NULL_HANDLE;
                other.allocation = {};
                other.usage = {};
                other.allocator = VK_NULL_HANDLE;
                other.size = 0;
            }

            return *this;
        }

    private:
        static vk::BufferUsageFlags getUsageFlags(BufferUsage usage);

    public:

        bool operator==(const Buffer& other) const {
            return size == other.size && usage == other.usage;
        }

    public:
        enum class MapError {
            SIZE_MISMATCH,
            NOT_CPU_VISIBLE
        };
        template<typename T = void>
        std::expected<MappedData<T>, MapError> map() {
            if constexpr (!std::is_same_v<T, void>) {
                if (sizeof(T) != this->size) return std::unexpected(Buffer::MapError::SIZE_MISMATCH);
            }
            
            MappedData<T> mapped;
            if (!mapped.Set(*this)) return std::unexpected(Buffer::MapError::NOT_CPU_VISIBLE);

            return mapped;
        }
};

template<typename T>
class MappedData {
    private:
        Buffer* buffer = nullptr;
        T* data = nullptr;

    public:
        MappedData() = default;
        ~MappedData() {
            UnSet();
        }

        MappedData(const MappedData&) = delete;
        void operator=(const MappedData&) = delete;

        MappedData(MappedData&& other) noexcept {
            this->buffer = other.buffer;
            this->data = other.data;

            other.buffer = nullptr;
            other.data = nullptr;
        }
        MappedData<T>& operator=(MappedData&& other) noexcept {
            if (this != &other) {
                UnSet();

                this->buffer = other.buffer;
                this->data = other.data;

                other.buffer = nullptr;
                other.data = nullptr;
            }

            return *this;
        }

    public:
        T& operator*() const { return *data; }
        T* operator->() const { return data; }
        explicit operator bool() const { return data; }

    private:
        void UnSet() {
            if (data && buffer) {
                vmaUnmapMemory(buffer->allocator, buffer->allocation);

                buffer = nullptr;
                data = nullptr;
            }
        }

    public:
        [[nodiscard]] bool Set(Buffer& _buffer) {
            this->buffer = &_buffer;

            void* raw;
            VkResult result = vmaMapMemory(buffer->allocator, buffer->allocation, &raw);

            if (result == VK_SUCCESS) {
                data = static_cast<T*>(raw);
                return true;
            }

            return false;
        }

        T* get() const { return data; }
};

template<>
class MappedData<void> {
private:
    Buffer* buffer = nullptr;
    void* data = nullptr;

public:
        MappedData() = default;
        ~MappedData() {
            UnSet();
        }

        MappedData(const MappedData&) = delete;
        void operator=(const MappedData&) = delete;

        MappedData(MappedData&& other) noexcept {
            this->buffer = other.buffer;
            this->data = other.data;

            other.buffer = nullptr;
            other.data = nullptr;
        }
        MappedData<void>& operator=(MappedData&& other) noexcept {
            if (this != &other) {
                UnSet();

                this->buffer = other.buffer;
                this->data = other.data;

                other.buffer = nullptr;
                other.data = nullptr;
            }

            return *this;
        }

    private:
        void UnSet() {
            if (data && buffer) {
                vmaUnmapMemory(buffer->allocator, buffer->allocation);

                buffer = nullptr;
                data = nullptr;
            }
        }

public:
    void* get() const { return data; }
    explicit operator bool() const { return data != nullptr; }

    bool Set(Buffer& buf) {
        buffer = &buf;

        void* raw = nullptr;
        VkResult result = vmaMapMemory(buffer->allocator, buffer->allocation, &raw);

        if (result != VK_SUCCESS) return false;

        data = raw;
        return true;
    }
};

using MappedDataRaw = MappedData<void>;

template<typename T>
concept Buffer_T = std::is_base_of_v<Buffer, T>;

class VertexBuffer : public Buffer {
    public:
        VertexBuffer() : Buffer(BufferUsage::VERTEX_BUFFER) {}
        VertexBuffer(size_t size) : Buffer(size, BufferUsage::VERTEX_BUFFER) {}
};

class IndexBuffer : public Buffer {
    friend class RenderContext;
    public:
        enum class IndexType {
            UINT_8,
            UINT_16,
            UINT_32 
        };

        IndexBuffer() : Buffer(BufferUsage::INDEX_BUFFER) {}
        IndexBuffer(size_t size, IndexType indexType = IndexBuffer::IndexType::UINT_8)
            : Buffer(size, BufferUsage::INDEX_BUFFER) {
                switch (indexType) {
                    case IndexType::UINT_8: this->indexType = vk::IndexType::eUint8; break;
                    case IndexType::UINT_16: this->indexType = vk::IndexType::eUint16; break;
                    case IndexType::UINT_32: this->indexType = vk::IndexType::eUint32; break;
                }
            }

    private:
        vk::IndexType indexType = vk::IndexType::eUint8;
};

class UniformBuffer : public Buffer {
    public:
        UniformBuffer() : Buffer(BufferUsage::UNIFORM_BUFFER) {}
        UniformBuffer(size_t size) : Buffer(size, BufferUsage::UNIFORM_BUFFER) {}
};

class TransferBuffer : public Buffer {
    public:
        TransferBuffer() : Buffer(BufferUsage::TRANSFER_BUFFER) {}
        TransferBuffer(size_t size) : Buffer(size, BufferUsage::TRANSFER_BUFFER) {}
};
