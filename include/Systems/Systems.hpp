#pragma once

#include <stack>
#include <memory>

class ISystem {
public:
	virtual ~ISystem() = default;

	virtual void Init() = 0;
	virtual void Cleanup() = 0;
};

class SystemManager {
private:
	std::stack<std::unique_ptr<ISystem>> systems;

public:
    template<typename T, typename... Args>
    T& PushSystem(Args&&... args) {
        auto system = std::make_unique<T>(std::forward<Args>(args)...);
        system->Init();
        T& ref = *system;
        systems.push(std::move(system));
        return ref;
    }

    void PopSystem() {
        if (!systems.empty()) {
            systems.top()->Cleanup();
            systems.pop();
        }
    }

    ~SystemManager() {
        while (!systems.empty()) {
            systems.top()->Cleanup();
            systems.pop();
        }
    }
};