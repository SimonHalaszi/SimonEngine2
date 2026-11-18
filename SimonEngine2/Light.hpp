#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "Object.hpp"
#include "Renderer.hpp"

#include "EngineMath.hpp"
#include "EngineRender.hpp"
#include "EngineUtil.hpp"


class Light : public Object {
	public:
		Light() = default;
		Light(const EngineMath::Transform& localTransform) { localTransform_ = localTransform; };

		void draw() const override final {};
		void update() override final {};

		virtual void sendLightInfo() const = 0;

	protected:
		void onStart() override;
		void onDestruction() override;
};
#endif