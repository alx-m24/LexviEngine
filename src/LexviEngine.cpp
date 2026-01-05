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

namespace Lexvi {
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

		return true;
	}

	void LexviEngine::Shutdown() {
		m_app->FullShutdown();
	}

    struct PhysicsCommand {
        uint32_t entityID = 0;
    };

    struct PhysicsBuffer {
        std::vector<glm::vec3> positions;
    };

    template<typename Derived>
    using PhyscisBase = Thread::SystemThreadWithQueue<Derived, PhysicsBuffer, 20u, PhysicsCommand>;
    class PhysicsThread : public PhyscisBase<PhysicsThread>
    {
        public:
            PhysicsThread() : PhyscisBase<PhysicsThread>("Physics") {}

        public:
            void Tick() {
                Log("Hi...");
            }
        
        protected:
            void ExecuteCommand(const PhysicsCommand& cmd) {

            }   
    };

    struct WeatherBuffer {};

    template<typename Derived>
    using WeatherBase = Thread::SystemThread<Derived, WeatherBuffer, 1u>;
    class WeatherThread : public WeatherBase<WeatherThread>
    {
        public:
            WeatherThread() : WeatherBase<WeatherThread>("Weather") {}

        public:
            void Tick() {
                Log("How is the weather???");
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
        WeatherThread weatherThread;

		while (m_app->isRunning() && m_window.isOpen()) {
            Time::Update();

			m_window.ProcessCallbacks();
            Input::CalculateDeltas();

			m_app->Update();

            Input::ClearFrameData();

			m_app->Render();

			m_window.SwapBuffers();
		}


        if (ResourcePool::isReady(firstHandle, intPool)) {
            Log("First value: {}", ResourcePool::Get(firstHandle, intPool).data);
        }
        else {
            Log("Value of index '{}' is not ready yet", firstHandle.id);
        }


        if (ResourcePool::isReady(floatHandle1, GetPool<float>(pools))) {
            Log("First float: {}", ResourcePool::Get(floatHandle1, GetPool<float>(pools)).data);
        }
        else {
            Log("Value of index '{}' in float is not ready yet", floatHandle1.id);
        }
	}
}
