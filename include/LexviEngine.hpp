#pragma once


#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <chrono>

#include <GLFW/glfw3.h>

#include <Game/Game.hpp>
#include <Input/Input.hpp>
#include <Renderer/Renderer.hpp>

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
	class Engine
	{
	private:
		std::unique_ptr<Game> game;
		Camera* currentCamera; // game own's camera

	private:
		GLFWwindow* window = nullptr;

		std::unique_ptr<Input> inputSystem;
		std::unique_ptr<Renderer> renderer;

	private:
		FrameTimers frameTimers;

		std::chrono::duration<double> TARGET_FRAME_DURATION{};

		bool PerformanceUI = true;
		int FPS_LIMIT = 0;

	public:
		Engine() = default;
		Engine(const std::string& title, std::unique_ptr<Game> newGame, bool VSYNC = false, bool PerformanceUI = true, int FPS_LIMIT = 0) { Init(title, std::move(newGame), VSYNC, PerformanceUI, FPS_LIMIT); };

		~Engine();

	public:
		void Init(const std::string& title, std::unique_ptr<Game> newGame, bool VSYNC = false, bool PerformanceUI = true, int FPS_LIMIT = 0);

	public:
		void run();

	public:
		void SetCurrentCamera(Camera* camera);
		void SetBackGroundColor(glm::vec3 color);
		void LockAndHideCursor();
		void ShowCursor();
		void ToggleCursorState();
		Input* getInputSystem() const;
		Renderer* getRenderer() const;

		void LockFPS(int FPS);

	private:
		void ShowEngineStats(float allocatedMB);
	};
}
