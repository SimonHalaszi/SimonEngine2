#include "Primitive.hpp"

#include "MaterialRegistry.hpp"

Primitive::Primitive(const EngineMath::Transform& localTransform, const EngineRender::Material* material) {
	if (material == nullptr) {
		material = MaterialRegistry::getInstance().getOrCreate("null", EngineRender::Material());
	}

	material_ = material;
	edgeMaterial_ = MaterialRegistry::getInstance().getOrCreate("Edge", EngineRender::Material());
	localTransform_ = localTransform;
}

void Primitive::setMaterial(const EngineRender::Material* material) {
	if (material == nullptr) {
		material = MaterialRegistry::getInstance().getOrCreate("null", EngineRender::Material());
	}
	material_ = material;
}

void Primitive::onStart() {
	resolveMeshes();
}

void Primitive::draw() const {
	if (GPUMesh_ == nullptr) {
		return;
	}

	if (drawAsSolid_) {
		Renderer::getInstance().drawMesh(
			*GPUMesh_,
			*material_,
			getWorldMatrix(),
			Renderer::DrawMode::Solid
		);
	}
	else {
		Renderer::getInstance().drawMesh(
			*GPUMesh_,
			*edgeMaterial_,
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
	material_ = nullptr;
}