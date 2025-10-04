#pragma once

#include "Renderer/Renderer.hpp"

class Game
{
public:
	virtual ~Game() = default;

    // Called once at the very start, before main loop
    virtual bool loadResources(Lexvi::Engine& engine) = 0;

    // Called once per frame, deltaTime in seconds
    virtual void update(Lexvi::Engine& engine, float deltaTime) = 0;

    // Called once per frame, engine provides Renderer / Scene abstraction
    virtual void render(Lexvi::Renderer& renderer) = 0;

    // Cleanup
    virtual void shutdown() {}
};

