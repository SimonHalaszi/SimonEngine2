#include "Primitive.hpp"

Primitive::Primitive(
    const EngineMath::Transform& localTransform,
    const EngineUtil::ColorRGB& color,
    const std::string& vertPath,
    const std::string& fragPath
) {
	vertPath_ = vertPath;
	fragPath_ = fragPath;
	localTransform_ = localTransform;
    material_.color_ = color;
	edgeMaterial_.color_ = { 255,255,255 };
}

void Primitive::onStart() {
	material_.shader_ = Renderer::getInstance().getOrLoadShader(vertPath_, fragPath_);
	edgeMaterial_.shader_ = Renderer::getInstance().getOrLoadShader("../Shaders/Basic.vert", "../Shaders/Color.frag");

	material_.shader_ = Renderer::getInstance().getOrLoadShader(vertPath_, fragPath_);

	resolveMeshes();
}

void Primitive::draw() const {
	if (GPUMesh_ == nullptr) {
		return;
	}

	if (drawAsSolid_) {
		Renderer::getInstance().drawMesh(
			*GPUMesh_,
			material_,
			getWorldMatrix(),
			Renderer::DrawMode::Solid
		);
	}
	else {
		Renderer::getInstance().drawMesh(
			*GPUMesh_,
			edgeMaterial_,
			getWorldMatrix(),
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

void Primitive::onDestruction() {
	mesh_ = nullptr;
	GPUMesh_ = nullptr;
}