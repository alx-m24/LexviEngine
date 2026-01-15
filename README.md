# LexviEngine

**LexviEngine** is a high-performance, fully threaded C++ game engine built around an **Entity-Component-System (ECS) architecture**. Beyond ECS and multithreading, LexviEngine provides a **flexible App framework** with layers, contexts, and a built-in stack system, designed for modular and scalable game or app development. Vulkan rendering support is in progress.

The engine maximizes CPU utilization while keeping the main thread non-blocking, enabling developers to focus on gameplay, app logic, and visuals.

## Key Features

- **Entity-Component-System (ECS)**: Clean separation of entities, components, and systems for modular and maintainable code.  
- **SmartThread & Worker System**: Dynamic threading system optimized for multi-core CPUs.  
- **SystemThreads & Command Threads**: Execute systems asynchronously with optional command-based control.  
- **Thread-Safe Logging, IO, and Resource Pool**: All core operations are safe across threads.  
- **Non-Blocking Main Thread**: Main thread only receives handles to completed tasks; it never waits.  
- **Compile-Time CRTP Inheritance**: Maximum performance, type safety, and minimal runtime overhead.  
- **App Framework**:  
  - **App Base Class** – Inherit from this to create your own apps.  
  - **App Layers & Contexts** – Organize functionality modularly.  
  - **Built-in App Stack** – Push/pop apps and layers easily.  
  - **Optional Transitions** – Smooth animated layer push/pop transitions.  
- **Future Vulkan Renderer** – Fully integrated with engine architecture and threading.

## Example Usage

```cpp
#include <LexviEngine/Input/Input.hpp>
#include <LexviEngine/LexviEngine.hpp>

#include <LexviEngine/pch.h>

#include "App.hpp"

using namespace Lexvi;

int main() {
  Log("[Demo] Not Lexvi Yet");

	LexviEngine engine(std::make_unique<Demo>());
	if (!engine.Init()) {
		Log("[Demo] Failed to init LexviEngine");
		return -1;
	}

	engine.Run();
	
	engine.Shutdown();
}
```

```cpp
// App
#pragma once

#include <LexviEngine/Application/Application.hpp>
#include <LexviEngine/Threading/SystemThread.hpp>
#include <LexviEngine/Input/Input.hpp>

#include "Layers.hpp"
#include "Contexts.hpp"
#include "Physics.hpp"

using namespace Lexvi;

class Demo : public Application {
	private:
		std::shared_ptr<MainContext> m_mainContext;

    private:
        int m_currentLayer = 0; // 0 = red, 1 = green
        size_t m_currentPhysicsBufferVersion = 0;
        std::optional<PhysicsOutBuffer> m_physicsState;

	public:
		void Init() override {
            m_mainContext = std::make_shared<MainContext>();
            m_mainContext->WindowSize = m_window->getSize();

            m_AppStack.AddApplicationLayer(CreateApplicationLayer<SimulationLayer>(m_mainContext));
		}

		void Shutdown() override {
		}

	protected:
		void AppUpdate() override {
            m_mainContext->WindowSize = m_window->getSize();

			if (Input::wasKeyPressed(Key::SPACE)) {
				m_currentLayer = (m_currentLayer + 1) % 2;

				switch (m_currentLayer) {
					case 0: m_AppStack.PopApplicationLayer(); break;
					case 1: m_AppStack.AddApplicationLayer(CreateApplicationLayer<PauseLayer>(m_mainContext)); break;
				}
			}	

            m_physicsState = m_mainContext->physicsThread.tryGetLatest(m_currentPhysicsBufferVersion);
            if (m_physicsState) {
                // Update ECS
                for (size_t i{0}; i < m_physicsState->entities.size(); ++i) {
                    const ECS::Entity& e = m_physicsState->entities[i];
                    
                    const Position& p = std::get<std::vector<Position>>(m_physicsState->components)[i];
                    const PhysicsComponent& pc = std::get<std::vector<PhysicsComponent>>(m_physicsState->components)[i];
                    const LifeTime& lt = std::get<std::vector<LifeTime>>(m_physicsState->components)[i];

                    if (lt.lifeTimeRemaining <= 0.0f) {
                        m_mainContext->ecs.DestroyEntity(e);
                        Log("Entity '{}, {}' died", e.id, e.generation);
                        continue;
                    }

                    m_mainContext->ecs.getComponent<Position>(e) = p;
                    m_mainContext->ecs.getComponent<PhysicsComponent>(e) = pc;
                    m_mainContext->ecs.getComponent<LifeTime>(e) = lt;
                }
            }
		}
};

