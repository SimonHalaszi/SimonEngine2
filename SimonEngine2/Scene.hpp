#ifndef SCENE_HPP
#define SCENE_HPP

#include <memory>
#include <stdexcept>
#include <vector>

#include "EngineUtil.hpp"

#include "Object.hpp"
#include "Light.hpp"
#include "CubeMap.hpp"
#include "Skybox.hpp"
#include "Camera.hpp"

// Scene Class

class Scene {
	public:
		static constexpr int uncappedFrameRate = -1;

		Scene(int updatesPerSecond = 20, int framesPerSecond = 244)
			: updatesPerSecond_(updatesPerSecond), framesPerSeconds_(framesPerSecond) {
			if (updatesPerSecond_ <= 0 ||
				(framesPerSeconds_ <= 0 && framesPerSeconds_ != uncappedFrameRate)) {
				throw std::invalid_argument("Scene update rate must be positive and frame rate must be positive or uncapped");
			}
		}

		virtual ~Scene() {}

		void addRootObject(std::unique_ptr<Object> gameObject);
		
		void sceneInit();

		void sceneDeInit();

		void sceneSubmit() const;

		void sceneUpdate();

		int getUpdateSpeed() const { return updatesPerSecond_; }
		int getFrameSpeed() const { return framesPerSeconds_; }

		void willDraw(bool willDraw) { isDrawing_ = willDraw; }
		void willUpdate(bool willUpdate) { isUpdating_ = willUpdate; }
		void setPauseFlag(bool pauseFlag) { pauseFlag_ = pauseFlag; }

		bool isDrawing() const { return isDrawing_; }
		bool isUpdating() const { return isUpdating_; }
		bool isPauseFlagged() const { return pauseFlag_; }

		Camera& getCamera() { return camera_; }
		const Camera& getCamera() const { return camera_; }

		void addLight(Light* light);
		void removeLight(Light* light);

		void addCubeMap(CubeMap* cubeMap);
		void removeCubeMap(CubeMap* cubeMap);

		void addSkybox(Skybox* skybox);
		void removeSkybox(Skybox* skybox);

		void setAmbientColor(const EngineUtil::ColorRGB& color) { ambientColor_ = color; }
		void setAmbientIntensity(float intensity) { ambientIntensity_ = intensity; }

		const EngineUtil::ColorRGB& getAmbientColor() const { return ambientColor_; }
		float getAmbientIntensity() const { return ambientIntensity_; }

	protected:
		// Scene specific functionalities handled here. GameObjects are updated AUTOMATICALLY based on per GameObject logic
		virtual void init() {} // Scene specific init (Stuff not attached to GameObjects)
		virtual void deInit() {} // Scene specific deInit (Stuff not attached to GameObjects)
		virtual void draw() const {} // Scene specific drawing (Stuff not attached to GameObjects)
		virtual void update() {} // Scene specific updating (Stuff not attached to GameObjects)

		Camera camera_;

		// Scene root GameObjects
		std::vector<std::unique_ptr<Object>> rootObjects_;
		std::vector<std::unique_ptr<Object>> pendingRootObjects_;
		
		// Non-owning pointers to keep track of light/environmental objects so that they can be handled first in rendering pass
		std::vector<Light*> lightObjects_;
		std::vector<CubeMap*> cubeMapObjects_;
		Skybox* skybox_ = nullptr;

		// Environmental settings
		EngineUtil::ColorRGB ambientColor_ = EngineUtil::ColorRGB(255, 255, 255);
		float ambientIntensity_ = 0.05f;

		bool isDrawing_ = true, isUpdating_ = true, pauseFlag_ = false;

		// Update Tick Speeds
		int updatesPerSecond_, framesPerSeconds_;

		bool isCurrentlyUpdating_ = false;

		void adoptRootObject(std::unique_ptr<Object> object);
		void processPendingRootObjects();
};

#endif