#ifndef ROBOT_SCENE_HPP
#define ROBOT_SCENE_HPP

#include "EngineMath.hpp"
#include "EngineRender.hpp"

#include "Scene.hpp"

class RobotScene : public Scene {
	public:
		RobotScene() : Scene(244, 244), robotPather_(nullptr), cubemap_(nullptr), pathOption_(0) {}

		~RobotScene() {}

	protected:
		virtual void init() override final;

		virtual void draw() const override final;

		virtual void update() override final;

		virtual void deInit() override final;

	private:
		void buildDemo(EngineMath::Transform transform, const EngineRender::Material* material);
		void changePath();

		void setUpNightEnvironment();
		void setUpBaseEnvironment();
		void setUpCloudyEnvironment();

		Object* robotPather_;
		Object* cubemap_;
		int pathOption_;
};

#endif