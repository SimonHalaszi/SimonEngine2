#ifndef PRIMITIVE_HPP
#define PRIMITIVE_HPP

#include "Object.hpp"
#include "InputManager.hpp"
#include "Renderer.hpp"

#include "EngineMath.hpp"
#include "EngineRender.hpp"
#include "EngineUtil.hpp"

#include "MeshRegistry.hpp"

class Primitive : public Object {
	public:
		Primitive() = default;
		Primitive(
			const EngineMath::Transform& localTransform, 
			const EngineUtil::ColorRGB& color, 
			const std::string& vertPath = "../Shaders/Basic.vert", 
			const std::string& fragPath= "../Shaders/Color.frag" 
		);

		void draw() const override final;
		void update() override final;

		void setColor(const EngineUtil::ColorRGB& color) { material_.color_ = color; }

	protected:
		bool drawAsSolid_ = true;
		const EngineUtil::Mesh* mesh_ = nullptr;
		const EngineRender::GPUMesh* GPUMesh_ = nullptr;
		EngineRender::Material material_;
};

#endif 
