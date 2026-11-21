#ifndef SCENE_HPP
#define SCENE_HPP

#include <string>
#include <memory>

#include "WindowConstants.hpp"
#include "Object.hpp"
#include "Light.hpp"
#include "CubeMap.hpp"
#include "Skybox.hpp"
#include "Camera.hpp"

#include "MaterialRegistry.hpp"
#include "MeshRegistry.hpp"
#include "TextureRegistry.hpp"
#include "Renderer.hpp"

// Scene Class

class Scene {
	public:
		Scene(int updatesPerSecond = 20, int framesPerSeconds = 244, int animationUpdatesPerSecond = 10) : 
			updatesPerSecond_(updatesPerSecond),
			framesPerSeconds_(framesPerSeconds),
			animationUpdatesPerSecond_(animationUpdatesPerSecond) 
		{}

		virtual ~Scene() {}

		void addRootObject(std::unique_ptr<Object> gameObject);
		void sceneInit();

		void sceneDeInit();

		void sceneDraw() const;

		void sceneUpdate();

		int getUpdateSpeed() const { return updatesPerSecond_; }
		int getFrameSpeed() const { return framesPerSeconds_; }
		int getAnimationUpdateSpeed() const { return animationUpdatesPerSecond_; }

		void willDraw(bool willDraw) { isDrawing_ = willDraw; }
		void willUpdate(bool willUpdate) { isUpdating_ = willUpdate; }
		void willUpdateAnimations(bool willUpdateAnimations) { isUpdatingAnimations_ = willUpdateAnimations; }
		void setPauseFlag(bool pauseFlag) { pauseFlag_ = pauseFlag; }

		bool isDrawing() const { return isDrawing_; }
		bool isUpdating() const { return isUpdating_; }
		bool isUpdatingAnimations() const { return isUpdatingAnimations_; }
		bool isPauseFlagged() const { return pauseFlag_; }

		void incrementAnimationFrame() { ++animationFrame_; }
		int getAnimationFrame() const { return animationFrame_; }

		void incrementUpdateFrame() { ++updateFrame_; }
		int getUpdateFrame() const { return updateFrame_; }

		Camera& getCamera() { return camera_; }
		const Camera& getCamera() const { return camera_; }

		void addLight(Light* light);
		void removeLight(Light* light);

		void addCubeMap(CubeMap* cubeMap);
		void removeCubeMap(CubeMap* cubeMap);

		void addSkybox(Skybox* skybox);
		void removeSkybox(Skybox* skybox);

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

		bool isDrawing_ = true, isUpdating_ = true, isUpdatingAnimations_ = true, pauseFlag_ = false;

		// Update Tick Speeds
		int updatesPerSecond_, framesPerSeconds_, animationUpdatesPerSecond_;
		int animationFrame_ = 0;
		int updateFrame_ = 0;

		bool isCurrentlyUpdating_ = false;

		void adoptRootObject(std::unique_ptr<Object> object);
		void processPendingRootObjects();
};

#endif // !GAME_HPP