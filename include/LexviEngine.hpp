#pragma once

#include <atomic>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <chrono>

class Game;       // forward declare
// Forward declare GLFWwindow so we don't include glfw3.h here
struct GLFWwindow;

#ifdef _DEBUG
static std::atomic<size_t> g_allocatedBytes = 0;
#endif


struct FrameTimers {
	float inputTime = 0.0f;
	float updateTime = 0.0f;
	float cameraTime = 0.0f;
	float renderTime = 0.0f;
	float guiTime = 0.0f;
};

namespace Lexvi {
	class Camera;     // forward declare
	class Input;      // forward declare
	class Renderer;   // forward declare

	class Engine
	{
	private:
		std::unique_ptr<Game> game;
		std::shared_ptr<Camera> currentCamera;

	private:
		GLFWwindow* window = nullptr;
		std::shared_ptr<Input> inputSystem;

		std::shared_ptr<Renderer> renderer;

	private:
		FrameTimers frameTimers;

	public:
		Engine() = default;
		Engine(const std::string& title, std::unique_ptr<Game> newGame) { Init(title, std::move(newGame)); };

		~Engine();

	public:
		void Init(const std::string& title, std::unique_ptr<Game> newGame);

	public:
		void run();

	public:
		void SetCurrentCamera(std::shared_ptr<Camera> camera);
		void SetBackGroundColor(glm::vec3 color);
		void LockAndHideCursor();
		void ShowCursor();
		void ToggleCursorState();
		std::shared_ptr<Input> getInputSystem() const;
		std::shared_ptr<Renderer> getRenderer() const;

	private:
		void ShowEngineStats(float allocatedMB);
	};
}
