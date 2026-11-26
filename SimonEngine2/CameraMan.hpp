#ifndef CAMERA_MAN_HPP
#define CAMERA_MAN_HPP

#include "EngineMath.hpp"

#include "Object.hpp"

class CameraMan : public Object {
	public:
		CameraMan(const EngineMath::Transform& localTransform);

	protected:
		void onStart() override final;
		void update() override final;

	private:
		float speed_;
};

#endif 
