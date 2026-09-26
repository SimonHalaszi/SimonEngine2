#ifndef BASIC_SCENE_HPP
#define BASIC_SCENE_HPP

// Needed C++ Includes
#include <string>
#include <memory>

#include <iostream>

// Scene Interface
#include "Scene.hpp"

#include "InputManager.hpp"

class BasicScene : public Scene {
	public:
		BasicScene() : Scene(244, 244, 10) {}

		~BasicScene() {}

		virtual void init() override final;

		virtual void draw() const override final;

		virtual void update() override final;
		
		static void mainMenu(int value);

};

#endif