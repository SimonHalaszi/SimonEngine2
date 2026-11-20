#ifndef ROBOT_SCENE_HPP
#define ROBOT_SCENE_HPP

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

class RobotScene : public Scene {
	public:
		RobotScene() : Scene(244, 244, 10), robotPather_(nullptr), pathOption_(0) {}

		~RobotScene() {}

	protected:
		virtual void init() override final;

		virtual void draw() const override final;

		virtual void update() override final;

		virtual void deInit() override final;

	private:
		void buildDemo(EngineMath::Transform transform, EngineRender::Material material);
		void changePath();
		Object* robotPather_;
		int pathOption_;
};

#endif