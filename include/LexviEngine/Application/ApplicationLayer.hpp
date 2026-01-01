#pragma once

#include <memory>

namespace Lexvi {
	// Forward declaration
	class ApplicationContext;

	class ApplicationLayer {
	    protected:
		    std::shared_ptr<ApplicationContext> m_Context;

	    public:
	    	ApplicationLayer(std::shared_ptr<ApplicationContext> context) : m_Context(context) {}

	    	// Not copyable or movable
	    	ApplicationLayer(const ApplicationLayer&) = delete;
	    	ApplicationLayer& operator=(const ApplicationLayer&) = delete;

	    	ApplicationLayer(ApplicationLayer&&) = delete;
	    	ApplicationLayer& operator=(ApplicationLayer&&) = delete;

	    	virtual ~ApplicationLayer() {}

	    	virtual void Init() = 0;

	    	virtual void Reload() = 0;

	    	virtual void Update() = 0;
	    	virtual void Render() = 0;

	    	virtual void Shutdown() = 0;

        public:
            enum class TransitionState : uint8_t {
                InProgress = 0,
                Completed
            };

            // Transitions (optional): returns true if transition over
            virtual TransitionState OnPush(float secondsSincePushed) { return TransitionState::Completed; }
            virtual TransitionState OnPop(float secondsSincePopped) { return TransitionState::Completed; }
	};

	template<typename T>
	concept IsApplicationLayer = std::is_base_of<ApplicationLayer, T>::value;

	template<typename T, typename... Args> requires IsApplicationLayer<T>
	inline std::unique_ptr<ApplicationLayer> CreateApplicationLayer(Args&&... args) {
		auto layer = std::make_unique<T>(std::forward<Args>(args)...);
		return layer;
	}
}
