#include "LexviEngine/pch.h"
#include "LexviEngine/LexviEngine.hpp"
#include "LexviEngine/Input/Input.hpp"
#include "LexviEngine/Time/Time.hpp"
#include "LexviEngine/Logging/Logging.hpp"
#include "LexviEngine/Threading/SmartThread.hpp"
#include "LexviEngine/Utils/GetSystemInfo.hpp"
#include "LexviEngine/Threading/ThreadRegistry.hpp"
#include "LexviEngine/ResourcePool/ResourcePool.hpp"
#include "LexviEngine/ResourcePool/Resource.hpp"
#include "LexviEngine/Threading/Worker.hpp"
#include "LexviEngine/Threading/SystemThread.hpp"
#include "LexviEngine/Threading/SystemThreadWithCommands.hpp"
#include "LexviEngine/ECS/ECS.hpp"

namespace Lexvi {

    class PositionComponent : public ECS::ComponentTag {
        public:
            glm::vec3 position;
    };

    class MeshComponent : public ECS::ComponentTag {};

	bool LexviEngine::Init() {	
		auto err = Thread::RegisterThread("Main");

		auto sysInfo_expected = getSystemInfo();

		if (!sysInfo_expected) {
			Log("Failed to get system info");
			return false;
		}
		SystemInfo sysInfo = *sysInfo_expected;
		Log("OpenGL version: {}.{}", sysInfo.glMajorVersion, sysInfo.glMinorVersion);

		auto windowError = m_window.Init(WindowInfo {
				.size = {800, 600},
				.title = "Test",
				.VSYNC = true,
				.visible = true,
				.systemInfo = sysInfo
				});

		if (windowError != WindowError::OK) {
			Log("{}{}", "Failed to init window: ", GetErrorString(windowError));
			return false;
		}

        Time::Init();

		m_app->Init();

        ECS::ECS<PositionComponent, MeshComponent> ecs{};
        
        auto e_err = ecs.CreateEntity<PositionComponent, MeshComponent>();

        if (!e_err) {
            ECS::AddError error = e_err.error();
            Log("{}", ECS::GetErrorString(error));
        }
        else {
            ECS::Entity first_e = *e_err;
            Log("First entity: id = {}, gen = {}", first_e.id, first_e.generation);
        }

        ECS::Entity e2 = *ecs.CreateEntity<PositionComponent, MeshComponent>();
        Log("Second entity: id = {}, gen = {}", e2.id, e2.generation);

        Log("Freeing first entity");

        ecs.DestroyEntity(*e_err);
        
        ECS::Entity e3 = *ecs.CreateEntity<PositionComponent>();
        Log("Third entity: id = {}, gen = {}", e3.id, e3.generation);
        
        Log("This entity has PositionComponent: {}", ecs.HasComponents<PositionComponent>(e3));
        Log("This entity has MeshComponent: {}", ecs.HasComponents<MeshComponent>(e3));
        Log("This entity has PositionComponent AND MeshComponent: {}", ecs.HasComponents<PositionComponent, MeshComponent>(e3));

        auto pos_err = ecs.getComponent<PositionComponent>(e3);
        if (!pos_err) {
            Log("{}", ECS::GetErrorString(pos_err.error()));
        }
        else {
            PositionComponent& pos = *pos_err;
            pos.position.x = 34.43f;
        }

        const PositionComponent& e3_pos = *ecs.getComponentConst<PositionComponent>(e3);
        Log("Third entity Position: {}, {}", e3_pos.position.x, e3_pos.position.y);

        Log("Testing Position only");
        ecs.ForEach<PositionComponent>([] (const ECS::Entity& e, PositionComponent& pos) {
                Log("Entity: id = {}, gen = {} -> position: ({}, {}, {})", e.id, e.generation, pos.position.x, pos.position.y, pos.position.z);
            }
        );
        
        Log("Testing Position and Mesh");
        ecs.ForEach<PositionComponent, MeshComponent>([] (const ECS::Entity& e, PositionComponent& pos, MeshComponent& mesh) {
                Log("Entity: id = {}, gen = {} -> position: ({}, {}, {}) && mesh: ({})", e.id, e.generation, pos.position.x, pos.position.y, pos.position.z, "No way to print mesh");
            }
        );

        ECS::Snapshot<PositionComponent> snapshot = ecs.getSnapshot<PositionComponent>();
        Log("Taking Snapshot of Positions");
        for (size_t i{0}; i < snapshot.entities.size(); ++i) {
            const ECS::Entity& e = snapshot.entities[i];
            const glm::vec3& pos = std::get<std::vector<PositionComponent>>(snapshot.components).at(i).position;
            Log("Entity: ({}, {}) -> Position: ({}, {}, {})", e.id, e.generation, pos.x, pos.y, pos.z);
        }

        Log("Taking Snapshot of Positions using past Snapshot by ref");
        ecs.getSnapshot<PositionComponent>(snapshot);
        for (size_t i{0}; i < snapshot.entities.size(); ++i) {
            const ECS::Entity& e = snapshot.entities[i];
            const glm::vec3& pos = std::get<std::vector<PositionComponent>>(snapshot.components).at(i).position;
            Log("Entity: ({}, {}) -> Position: ({}, {}, {})", e.id, e.generation, pos.x, pos.y, pos.z);
        }

        return false;

		return true;
	}

	void LexviEngine::Shutdown() {
		m_app->FullShutdown();
	}

    struct PhysicsCommand {
    };

    struct PhysicsBuffer {
        uint64_t frameNum = 0;
    };

    template<typename Derived>
    using PhyscisBase = Thread::SystemThreadWithQueue<Derived, PhysicsBuffer, 200u, PhysicsCommand>;
    class PhysicsThread : public PhyscisBase<PhysicsThread> {
        public:
            PhysicsThread() : PhyscisBase<PhysicsThread>("Physics") {}

        private:
            uint64_t frameNum = 0;

        public:
            void Tick() {
                PhysicsBuffer& buffer = getWriteBuffer();

                buffer.frameNum = ++frameNum;
            }
        
        protected:
            void ExecuteCommand(const PhysicsCommand& cmd) {

            }   
    };

	void LexviEngine::Run() {
        Thread::SmartThread worker("CounterThread", [](std::stop_token st) {
            int i = 0;
            while (!st.stop_requested()) {
                Log("Count: {}", ++i);
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
            Log("Stop requested, exiting...");
        });
        // worker.RequestStop();

        using Pools = Lexvi::ResourcePool::ResourcePools<int, float>;
        using IntResource = ResourcePool::Resource<int>;
        using IntHandle = ResourcePool::ResourceHandle<int>;
        
        Pools pools;
        ResourcePool::ResourcePool<int>& intPool = GetPool<int>(pools);

        Thread::Worker<int, std::string> testWorker(
                "TestWorker",
                intPool,
                [](IntResource& res, const std::string& input) { 
                    Log("{}", input);
                    res.data = 69; 
                } 
            ); 

        Thread::Worker<float, std::string> testWorker2(
                "TestWorker2",
                GetPool<float>(pools),
                [](ResourcePool::Resource<float>& res, const std::string& input) {
                    Log("{}", input);
                    res.data = 69.69f;
                }
            );

        IntHandle firstHandle = testWorker.Submit("First job");
        Log("First Handle: {}", firstHandle.id);

        ResourcePool::ResourceHandle<float> floatHandle1 = testWorker2.Submit("Second job, first for float");
        Log("Float handle: {}", floatHandle1.id);

        if (ResourcePool::isReady(firstHandle, intPool)) {
            Log("First value: {}", ResourcePool::Get(firstHandle, intPool).data);
        }
        else {
            Log("Value of index '{}' in int is not ready yet", firstHandle.id);
        }
        

        if (ResourcePool::isReady(floatHandle1, GetPool<float>(pools))) {
            Log("First float: {}", ResourcePool::Get(floatHandle1, GetPool<float>(pools)).data);
        }
        else {
            Log("Value of index '{}' in float is not ready yet", floatHandle1.id);
        }

        PhysicsThread physicsThread;

		while (m_app->isRunning() && m_window.isOpen()) {
            Time::Update();

			m_window.ProcessCallbacks();
            Input::CalculateDeltas();

			m_app->Update();

            Input::ClearFrameData();

            PhysicsBuffer physicsBuffer = physicsThread.getLatest();
            Log("[Physics] FrameNum: {}", physicsBuffer.frameNum);

			m_app->Render();

			m_window.SwapBuffers();
		}


        ResourcePool::FreeResource(firstHandle, intPool);
        auto newHandle = testWorker.Submit("Second job");

        if (!ResourcePool::isValid(firstHandle, intPool)) {
            Log("Value of index '{}' and generation '{}' is not valid", firstHandle.id, firstHandle.generation);
        }
        else {
            Log("First value: {}", ResourcePool::Get(firstHandle, intPool).data);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        if (!ResourcePool::isReady(newHandle, intPool)) {
            Log("Value of index '{}' and generation '{}' is not ready", newHandle.id, newHandle.generation);
        }
        else if (!ResourcePool::isValid(newHandle, intPool)) {
            Log("Value of index '{}' and generation '{}' is not valid", newHandle.id, newHandle.generation);
        }
        else {
            Log("Second value: {}", ResourcePool::Get(newHandle, intPool).data);
        }


        if (ResourcePool::isReady(floatHandle1, GetPool<float>(pools))) {
            Log("First float: {}", ResourcePool::Get(floatHandle1, GetPool<float>(pools)).data);
        }
        else {
            Log("Value of index '{}' in float is not ready yet", floatHandle1.id);
        }
	}
}
