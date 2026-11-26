#ifndef ARCBALL_CAMERA_HPP
#define ARCBALL_CAMERA_HPP

#include "EngineMath.hpp"

#include "Object.hpp"

class ArcballCamera : public Object {
	public:
		ArcballCamera(EngineMath::Vector3 target = {}, float distance = 10.0f, float speed = 5.0f);

		void setSpeed(float speed) { speed_ = speed; }

	protected:
		void onStart() override final;
		void update() override final;

	private:
		void applyCamera();

		float distance_;
		float speed_;
		float yaw_ = 0.0f;
		float pitch_ = -EngineMath::PI / 4.0f;
};

#endif 
