#ifndef PROPER_PRIMITIVE_HPP
#define PROPER_PRIMITIVE_HPP

#include <string>

#include "Object.hpp"
#include "InputManager.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"
#include "Renderer.hpp"
#include "MeshRegistry.hpp"

class ProperPrim : public Object {
	public:
		ProperPrim() = default;
		ProperPrim(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color, const std::string& vertPath, const std::string& fragPath);

		void draw() override;
		void update() override;

		void setColor(const EngineUtil::ColorRGB& color) { material_.color_ = color; }

	protected:
		bool drawAsSolid_ = true;
		const GPUMesh* mesh_ = nullptr;
		EngineUtil::Material material_;
};

#endif 
