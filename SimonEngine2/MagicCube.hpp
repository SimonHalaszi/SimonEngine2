#ifndef MAGIC_CUBE_HPP
#define MAGIC_CUBE_HPP

#include "Object.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"

class MagicCube : public Object {
	public:
		MagicCube() = default;
		MagicCube(const EngineMath::Transform& localTransform);

		void onStart() override;
		void update() override;

	private:

		float rotationRadians_ = 0.0f;
		float rotationSpeed_ = 0.02f;
};

#endif 
