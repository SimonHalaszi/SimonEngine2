#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <chrono>
#include <memory>

#include "Scene.hpp"

// Engine Class
class Engine {
	public:
		static Engine& getInstance() {
			static Engine instance;
			return instance;
		}

		void init();

		const Scene* getCurrentScene() const { return currentScene_.get(); }
		Scene* getCurrentScene() { return currentScene_.get(); }
		void changeScene(std::unique_ptr<Scene> newScene);

		void safeExit();

		// Public only so buffer functions can call them
		void schedulerTimer(int v);

		float getDeltaTime() const { return deltaTime_; }
		float getFPS() const { return fps_; }
		void recordDrawnFrame();
		float getTime() const { return time_; }
		void refreshTime();

		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;
		Engine(Engine&&) = delete;
		Engine& operator=(Engine&&) = delete;

	private:
		bool applyPendingScene();

		// init functions
		void createWindow();
		void initOpenGL();
		void printGraphicsInfo() const;
		void registerInputFuncs();
		void registerTimeFuncs();

		void configureFrameScheduling();

		Engine() : currentScene_(nullptr), pendingScene_(nullptr) {}

		~Engine() {}

		std::unique_ptr<Scene> currentScene_;
		std::unique_ptr<Scene> pendingScene_;

		std::chrono::steady_clock::time_point startTime_{};
		bool hasStartTime_ = false;
		float time_ = 0.0f;

		std::chrono::steady_clock::time_point nextUpdateTime_{};
		std::chrono::steady_clock::time_point nextFrameTime_{};
		float deltaTime_ = 0.0f;

		std::chrono::steady_clock::time_point fpsMeasurementStart_ = std::chrono::steady_clock::now();
		unsigned int framesDrawn_ = 0;
		float fps_ = 0.0f;
};

#endif
