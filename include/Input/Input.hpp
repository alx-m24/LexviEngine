#pragma once

#include <functional>
#include <GLFW/glfw3.h>

namespace Lexvi {
	class Input {
	public:
		enum InputEvent {
			RESIZED,
		};

		struct ResizeEvent {
			int width, height;
		};

		using ResizeCallback = std::function<void(const ResizeEvent&)>;

		std::vector<ResizeCallback> resizeCallbacks;

	private:
		uint32_t SCR_WIDTH = 1200;
		uint32_t SCR_HEIGHT = 750;

	private:
		glm::vec2 mouseDelta = glm::vec2(0.0f);
		glm::vec2 mousePos = glm::vec2(0.0f);
		glm::vec2 lastmousePos = glm::vec2(0.0f);
		float scrollDeltaY = 0.0f;

	private:
		bool firstMouse = true;

	private:
		struct KeyState {
			bool wasPressed;
			bool isHeld;
			bool isPressed;
		};
		using MouseButtonState = KeyState;

		std::unordered_map<int, KeyState> keys;
		std::unordered_map<int, MouseButtonState> mouseButtons;

	private:
		KeyState getKeyState(int key) const;
		MouseButtonState getMouseButtonState(int button) const;

	public:
		Input() = default;

	public:
		void Update();

	public:
		// Checks if key is held accross frames
		bool isKeyHeld(int key) const;
		// Checks if key is being held repeatedly OS behavious
		bool isKeyRepeat(int key) const;
		// Checks if key just got released
		bool isKeyReleased(int key) const;
		// Checks if key just started being pressed
		bool isKeyStartedHold(int key) const;

	public:
		// Checks if button just started being pressed
		bool isMouseButtonPressed(int button) const;
		// Checks if button is held accross frames
		bool isMouseButtonHeld(int button) const;
		// Checks if button just got released
		bool isMouseButtonReleased(int button) const;

		glm::vec2 getMousePosition() const;

		glm::vec2 getMouseDelta() const;

		float getScrollWheelDelta() const;

	public:
		uint32_t getScreenWidth() const;

		uint32_t getScreenHeight() const;

	public:
		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

		static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

		static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

		static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

		static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

	public:
		inline void AddEvent(ResizeCallback callback) {
			resizeCallbacks.push_back(std::move(callback));
		}
	};
}