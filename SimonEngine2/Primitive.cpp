#include "Primitive.hpp"

Primitive::Primitive(const EngineMath::Transform& localTransform, EngineRender::Material material)
: material_(std::move(material)) {
	localTransform_ = localTransform;
}

void Primitive::setMaterial(EngineRender::Material material) {
	material.init();
	material_ = std::move(material);
}

void Primitive::onStart() {
	material_.init();
	edgeMaterial_.init();

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