#include "pch.h"

#include "LexviEngine.hpp"

#include "Game/Game.hpp"
#include "Input/Input.hpp"
#include "Camera/Camera.hpp"
#include "Renderer/Renderer.hpp"

#ifdef _DEBUG
void* operator new(std::size_t sz) {
	g_allocatedBytes += sz;
	if (void* ptr = std::malloc(sz)) return ptr;
	throw std::bad_alloc{};
}

void operator delete(void* ptr, std::size_t sz) noexcept {
	g_allocatedBytes -= sz;
	std::free(ptr);
}
#endif

using namespace Lexvi;

Lexvi::Engine::~Engine()
{
	if (window) {
		glfwDestroyWindow(window);
		glfwTerminate();
	}
	if (game) {
		game->shutdown();
	}
}

void Engine::Init(const std::string& title, std::unique_ptr<Game> newGame, bool VSYNC, bool PerformanceUI, int FPS_LIMIT)
{
	this->PerformanceUI = PerformanceUI;

	this->FPS_LIMIT = FPS_LIMIT;
	this->LockFPS(FPS_LIMIT);

	// if game already define -> setup/init already called
	if (game && window) return;

	// move ownership of game to engine
	game = std::move(newGame);

	inputSystem = std::make_unique<Input>();
	renderer = std::make_unique<Renderer>();

	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialize GLFW");
	}

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(inputSystem->getScreenWidth(), inputSystem->getScreenHeight(), title.c_str(), nullptr, nullptr);
	if (window == nullptr) {
		throw std::runtime_error("Failed to create window");
		return;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval((int)VSYNC);

	glfwSetWindowUserPointer(window, inputSystem.get());
	glfwSetKeyCallback(window, Lexvi::Input::keyCallback);
	glfwSetCursorPosCallback(window, Lexvi::Input::mouse_callback);
	glfwSetMouseButtonCallback(window, Lexvi::Input::mouseButtonCallback);
	glfwSetScrollCallback(window, Lexvi::Input::scroll_callback);
	glfwSetFramebufferSizeCallback(window, Lexvi::Input::framebuffer_size_callback);

	stbi_set_flip_vertically_on_load(true);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		throw std::runtime_error("Failed to initialize GLAD");
		return;
	}

	inputSystem->framebuffer_size_callback(window, inputSystem->getScreenWidth(), inputSystem->getScreenHeight());

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	ImPlot::CreateContext();
}

void Engine::run()
{
	if (!game) {
		throw std::runtime_error("Engine::run called without a Game set!");
		return;
	}

	game->loadResources(*this);

	using clock = std::chrono::high_resolution_clock;

	float lastFrameTime = static_cast<float>(glfwGetTime());
	while (!glfwWindowShouldClose(window)) {
		auto frameStart = clock::now();
		float frameTime = static_cast<float>(glfwGetTime());
		float dt = frameTime - lastFrameTime;
		lastFrameTime = frameTime;

		inputSystem->Update();

		game->update(*this, dt);

		if (currentCamera) currentCamera->update(dt);

		renderer->ClearBuffers();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		game->render(*renderer);

		if (PerformanceUI) {
#ifdef _DEBUG
			float allocatedMB = g_allocatedBytes.load() / 1'000'000.0f;
			ShowEngineStats(allocatedMB);
#else
			ShowEngineStats(0.0f);
#endif
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);

		if (FPS_LIMIT > 0) {
			auto frameEnd = clock::now();
			auto frameDuration = frameEnd - frameStart;

			auto sleepTime = TARGET_FRAME_DURATION - frameDuration;
			if (sleepTime > std::chrono::duration<double>(0)) {
				std::this_thread::sleep_for(sleepTime);
			}

			// busy-wait a few microseconds for more precise timing
			while ((clock::now() - frameStart) < TARGET_FRAME_DURATION) {}
		}

	}

	game->shutdown();

	ImPlot::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();

	window = nullptr;
}

void Lexvi::Engine::SetCurrentCamera(Camera* camera)
{
	if (!camera) {
		return;
	}

	camera->SetInputSystem(inputSystem.get());
	currentCamera = camera;
}

void Lexvi::Engine::SetBackGroundColor(glm::vec3 color)
{
	glClearColor(color.r, color.g, color.b, 1.0f);
}

Input* Lexvi::Engine::getInputSystem() const
{
	return inputSystem.get();
}

Renderer* Lexvi::Engine::getRenderer() const
{
	return renderer.get();
}

void  Lexvi::Engine::LockFPS(int FPS) {
	this->FPS_LIMIT = FPS;

	TARGET_FRAME_DURATION = std::chrono::duration<double>(1.0 / FPS_LIMIT);
}

void Lexvi::Engine::ShowEngineStats(float allocatedMB)
{
	ImGui::SetNextWindowBgAlpha(0.3f); // translucent background
	ImGui::Begin("Engine Stats", nullptr,
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoCollapse);

	ImGuiIO& io = ImGui::GetIO();
	float fps = io.Framerate;
	float frameTime = io.DeltaTime * 1000.0f; // ms

	// Color code FPS
	ImVec4 fpsColor = fps >= 60.0f ? ImVec4(0, 1, 0, 1)
		: fps >= 30.0f ? ImVec4(1, 1, 0, 1)
		: ImVec4(1, 0, 0, 1);

	// Top row: FPS | FrameTime | Memory
	ImGui::Text("FPS: "); ImGui::SameLine();
	ImGui::TextColored(fpsColor, "%.0f", fps);
	ImGui::SameLine(120); // spacing
	ImGui::Text("Frame: %.2f ms", frameTime);

	if (allocatedMB) {
		ImGui::SameLine(250);
		ImGui::Text("Mem: %.2f MB", allocatedMB);
	}

	// Optional small bar to visualize FPS relative to 60
	float barWidth = glm::clamp(fps / 60.0f, 0.0f, 1.0f);
	ImVec2 size(200, 10);
	ImGui::Dummy(size); // spacing for bar
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImVec2 pos = ImGui::GetItemRectMin();
	draw_list->AddRectFilled(pos, ImVec2(pos.x + size.x * barWidth, pos.y + size.y),
		IM_COL32((int)(255 * (1 - barWidth)), (int)(255 * barWidth), 0, 255));
	draw_list->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(255, 255, 255, 150));

	ImGui::End();
}

void Lexvi::Engine::LockAndHideCursor() {
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Lexvi::Engine::ShowCursor() {
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Lexvi::Engine::ToggleCursorState() {
	int state = glfwGetInputMode(window, GLFW_CURSOR);
	if (state == GLFW_CURSOR_DISABLED)
		ShowCursor();
	else
		LockAndHideCursor();

}