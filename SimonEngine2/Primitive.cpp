#include "Primitive.hpp"

Primitive::Primitive(
    const EngineMath::Transform& localTransform,
    const EngineUtil::ColorRGB& color,
    const std::string& vertPath,
    const std::string& fragPath
) {
	localTransform_ = localTransform;
    material_.color_ = color;
    material_.shader_ = Renderer::getInstance().getOrLoadShader(vertPath, fragPath);
}

void Primitive::draw() const {
	if (GPUMesh_ == nullptr) {
		return;
	}

	if (drawAsSolid_) {
		Renderer::getInstance().drawMesh(
			*GPUMesh_,
			material_,
			getWorldTransform().toMatrix(),
			Renderer::DrawMode::Solid
		);
	}
	else {
		Renderer::getInstance().drawMesh(
			*GPUMesh_,
			material_,
			getWorldTransform().toMatrix(),
			Renderer::DrawMode::Edges
		);
	}
}

void Primitive::update() {
    if (InputManager::getInstance().isPressed('1')) {
        drawAsSolid_ = false;
    }
    if (InputManager::getInstance().isPressed('2')) {
        drawAsSolid_ = true;
    }
}