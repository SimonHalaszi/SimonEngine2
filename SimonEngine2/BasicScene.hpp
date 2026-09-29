#ifndef BASIC_SCENE_HPP
#define BASIC_SCENE_HPP

// Needed C++ Includes
#include <string>
#include <memory>

#include <iostream>

// Scene Interface
#include "Scene.hpp"

// Other SimonEngine includes
#include "InputManager.hpp"
#include "EngineUtil.hpp"

// Objects
#include "MagicCube.hpp"
#include "Robot.hpp"
#include "AxisDisplay.hpp"
#include "ObjMesh.hpp"

class BasicScene : public Scene {
	public:
		BasicScene() : Scene(244, 244, 10) {}

		~BasicScene() {}

		virtual void init() override final;

		virtual void draw() const override final;

		virtual void update() override final;

		virtual void deInit() override final;
		
		void mainMenu(int value);

	private:
		Robot* getRobot();

		int rotationXMenu_ = 0;
		int rotationYMenu_ = 0;
		int rotationZMenu_ = 0;
		int rotationMenu_ = 0;

		int perspectiveMenu_ = 0;
		int renderMenu_ = 0;

		int colorMenu_ = 0;

		int mainMenu_ = 0;
};

#endif