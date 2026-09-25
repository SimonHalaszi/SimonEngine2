#ifndef MAGIC_CUBE_HPP
#define MAGIC_CUBE_HPP

#include "Object.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"

class MagicCube : public Object {
public:
	MagicCube() = default;
	MagicCube(const EngineMath::Transform& transform);

	void onStart() override;
	void draw() override {}
	void update() override;
	void onDestruction() override {}

private:

	float rotationRadians_ = 0.0f;
	float rotationSpeed_ = 0.02f;
};

#endif 
