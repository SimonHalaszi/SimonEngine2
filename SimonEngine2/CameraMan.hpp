#ifndef CAMERA_MAN_HPP
#define CAMERA_MAN_HPP

#include "Object.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"
#include "InputManager.hpp"

class CameraMan : public Object {
	public:
		CameraMan() = default;
		CameraMan(const EngineMath::Transform& localTransform);

		void onStart() override final;
		void update() override final;

	private:
		float speed_;
};

#endif 
