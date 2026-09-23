#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <GL/glut.h>
#include <GL/freeglut.h>

#include <string>
#include <memory>
#include <iostream>

#include "Scene.hpp"
#include "InputManager.hpp"

// Engine Class
class Engine {
	public:
		static Engine& getInstance() {
			static Engine instance;
			return instance;
		}

		void init();

		const Scene* getCurrentScene() { return currentScene_.get(); }
		void changeScene(std::unique_ptr<Scene> newScene);

		void safeExit();

		// Public only so buffer functions can call them
		void frameTimer(int v);
		void updateTimer(int v);
		void animationTimer(int v);

		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;
		Engine(const Engine&&) = delete;
		Engine& operator=(const Engine&&) = delete;

	private:
		Engine() : currentScene_(nullptr), pendingScene_(nullptr) {}

		~Engine() {}

		std::unique_ptr<Scene> currentScene_;
		std::unique_ptr<Scene> pendingScene_;
};

#endif
