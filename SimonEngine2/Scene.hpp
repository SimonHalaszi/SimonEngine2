#ifndef SCENE_HPP
#define SCENE_HPP

#include <string>
#include <memory>

#include "WindowConstants.hpp"
#include "Object.hpp"

// Scene Class

class Scene {
	public:
		Scene(int updatesPerSecond = 244, int framesPerSeconds = 244, int animationUpdatesPerSecond = 10) : 
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

		void setProjectionOrtho(double, double, double, double, double, double) const;
		void setProjectionPerspective(double, double, double) const;

		std::vector<std::unique_ptr<Object>>* getRootObjects() { return &rootObjects_; }

	protected:
		// Scene specific functionalities handled here. GameObjects are updated AUTOMATICALLY based on per GameObject logic
		virtual void init() {} // Scene specific init (Stuff not attached to GameObjects)
		virtual void deInit() {} // Scene specific deInit (Stuff not attached to GameObjects)
		virtual void draw() const {} // Scene specific drawing (Stuff not attached to GameObjects)
		virtual void update() {} // Scene specific updating (Stuff not attached to GameObjects)

		// Scene root GameObjects
		std::vector<std::unique_ptr<Object>> rootObjects_;
		std::vector<std::unique_ptr<Object>> pendingRootObjects_;

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