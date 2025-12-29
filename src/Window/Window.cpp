#include <LexviEngine/pch.h>
#include <LexviEngine/Window/Window.hpp>
#include <LexviEngine/Input/Input.hpp>

namespace Lexvi {
	Window::~Window() {
		if (m_window) DeleteWindow();
		glfwTerminate();
	}

	WindowError Window::Init(const WindowInfo& windowInfo) {
		m_windowInfo = windowInfo;	
		return CreateWindow();
	}


	WindowError Window::Init(WindowInfo&& windowInfo) {
		m_windowInfo = std::move(windowInfo);
		return CreateWindow();
	}

	WindowError Window::CreateWindow() {
		if (m_window) DeleteWindow();
		auto& systemInfo = m_windowInfo.systemInfo;
		if (!systemInfo.DSA_Support) return WindowError::DSA_NOT_SUPPORTED;

		if (!glfwInit()) {
		    return WindowError::GLFW_INIT_FAIL;
		}

		glfwWindowHint(GLFW_VISIBLE, m_windowInfo.visible ? GLFW_TRUE : GLFW_FALSE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, systemInfo.glMajorVersion);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, systemInfo.glMinorVersion);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_window = glfwCreateWindow(m_windowInfo.size.x, m_windowInfo.size.y, std::string(m_windowInfo.title).c_str(), nullptr, nullptr);
		if (!m_window) {
			return WindowError::GLFW_WINDOW_FAIL;
		}
		glfwMakeContextCurrent(m_window);
		glfwSwapInterval(static_cast<int>(m_windowInfo.VSYNC));
		
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			return WindowError::GLAD_INIT_FAIL;
		}

        this->setCallbacks();

		return WindowError::OK;
	}

    void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
        auto* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (!win) return;

        win->m_windowInfo.size = WindowSize(static_cast<float>(width), static_cast<float>(height));
    }

    void Window::setCallbacks() {
        glfwSetWindowUserPointer(m_window, this);

        glfwSetFramebufferSizeCallback(m_window, FramebufferSizeCallback);
        glfwSetCursorPosCallback(m_window, Input::MousePositionCallback);
        glfwSetKeyCallback(m_window, Input::keyCallback);
        glfwSetScrollCallback(m_window, Input::MouseScrollCallback);
        glfwSetMouseButtonCallback(m_window, Input::MouseButtonCallback);
    }

    WindowSize Window::getSize() const {
        return this->m_windowInfo.size;
    }

	void Window::DeleteWindow() {
		glfwDestroyWindow(m_window);
		m_window = nullptr;
	}

	bool Window::isOpen() const {
		return !glfwWindowShouldClose(m_window);
	}
    
    void Window::ProcessCallbacks() {
        glfwPollEvents();
	}

	void Window::SwapBuffers() const {
		glfwSwapBuffers(m_window);
	}
}
