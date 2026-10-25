#ifndef STRESS_TEST_SCENE_HPP
#define STRESS_TEST_SCENE_HPP

// Needed C++ Includes
#include <string>
#include <memory>
#include <iostream>

// Scene Interface
#include "Scene.hpp"

// Other SimonEngine includes
#include "InputManager.hpp"
#include "EngineUtil.hpp"

// Scene Specific C++
#include <random>

class StressTestScene : public Scene {
public:
	// Want an arbituarily high fps, test without vsync
	StressTestScene() : Scene(244, 1000000, 10) {}

	~StressTestScene() {}

	virtual void init() override final;

	virtual void draw() const override final;

	virtual void update() override final;

	virtual void deInit() override final;
};

#endif