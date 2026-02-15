#include "LexviEngine/pch.hpp"
#include "LexviEngine/Renderer/Renderer.hpp"
#include "LexviEngine/Renderer/Renderer-Exceptions.hpp"

void Renderer::CreateSurface() {
    VkSurfaceKHR _surface;
    if (glfwCreateWindowSurface(*m_instance, m_window, nullptr, &_surface) != 0) {
        throw CreateSurface_Error("Failed to create vkSurface");
    }

    m_surface = vk::raii::SurfaceKHR(m_instance, _surface);
}
