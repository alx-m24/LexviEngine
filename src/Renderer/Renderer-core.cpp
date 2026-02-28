#include "LexviEngine/pch.hpp"
#include "LexviEngine/Utils.hpp"
#include "LexviEngine/Input/Input.hpp"
#include "LexviEngine/Renderer/Renderer.hpp"
#include "LexviEngine/Renderer/Renderer-Exceptions.hpp"
#include "LexviEngine/Renderer/RenderGraph/RenderGraph.hpp"

namespace InitialValues {
    constexpr vk::Extent2D windowSize = {
        .width = 800,
        .height = 600
    };
}

Renderer::Renderer() = default;
Renderer::~Renderer() {
    Shutdown();
}

void Renderer::InitGLFW(const std::string& title) {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_window = glfwCreateWindow(InitialValues::windowSize.width, InitialValues::windowSize.height, title.c_str(), nullptr, nullptr);

    using namespace Lexvi;

    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, FrameBufferSizeCallback);
    glfwSetCursorPosCallback(m_window, Input::MousePositionCallback);
    glfwSetKeyCallback(m_window, Input::keyCallback);
    glfwSetScrollCallback(m_window, Input::MouseScrollCallback);
    glfwSetMouseButtonCallback(m_window, Input::MouseButtonCallback);
}

void Renderer::FrameBufferSizeCallback(GLFWwindow *window, int width, int height) {
    Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    renderer->m_frameBufferResized = true; 
    if (renderer->m_resizeCallback) {
        renderer->m_resizeCallback(glm::uvec2{ width, height }); 
    }
}

void Renderer::SetResizeCallback(ResizeCallBackFunc&& func) {
    this->m_resizeCallback = std::forward<ResizeCallBackFunc>(func);
}

Renderer::InitResult Renderer::Init(const std::string& title) {
    InitGLFW(title);

    try {
        CreateInstance(title);
        CreateSurface();
        PickPhysicalDevice();
        CreateLogicalDeviceAndQueues();
        CreateSwapChain();
        CreateCommandPool();
        CreateCommandBuffers();
        CreateSyncObjects();
        CreateAllocator();
    }
    catch (const CreateInstance_Error& e) {
        DEBUG_PRINT(e.what()); 
        return InitResult::INSTANCE_FAILED;
    }
    catch (const CreateSurface_Error& e) {
        DEBUG_PRINT(e.what()); 
        return InitResult::SURFACE_FAILED;
    }
    catch (const PickPhysicalDevice_Error& e) {
        DEBUG_PRINT(e.what()); 
        return InitResult::PICK_PHYSICAL_DEVICE_FAILED;
    }
    catch (const CreateLogicalDevice_Error& e) {
        DEBUG_PRINT(e.what()); 
        return InitResult::LOGICAL_DEVICE_FAILED;
    }
    catch (const CreateAllocatorError& e) {
        DEBUG_PRINT(e.what()); 
        return InitResult::ALLOCATOR_FAILED;
    }

    return InitResult::OK;
}

void Renderer::Update() {
    glfwPollEvents();
}

void Renderer::Shutdown() {
    WaitIdle();

    if (m_allocator) {
        vmaDestroyAllocator(m_allocator);
    }

    CleanupSwapChain();

    glfwDestroyWindow(m_window);

    glfwTerminate();
}

bool Renderer::isRunning() const {
    return !glfwWindowShouldClose(m_window);
}
