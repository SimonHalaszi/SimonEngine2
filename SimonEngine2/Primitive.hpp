#ifndef PRIMITIVE_HPP
#define PRIMITIVE_HPP

#include "Object.hpp"
#include "InputManager.hpp"
#include "Renderer.hpp"

#include "EngineMath.hpp"
#include "EngineRender.hpp"
#include "EngineUtil.hpp"

#include "MeshRegistry.hpp"
#include "TextureRegistry.hpp"

class Primitive : public Object {
	public:
		Primitive() = default;
		Primitive(const EngineMath::Transform& localTransform, const EngineRender::Material* material = nullptr);

		void setMaterial(const EngineRender::Material* material);

	protected:
		void draw() const override;
		void update() override final;

		bool drawAsSolid_ = true;
		
		const EngineUtil::Mesh* mesh_ = nullptr;
		const EngineRender::GPUMesh* GPUMesh_ = nullptr;
		
		const EngineRender::Material* material_;
		const EngineRender::Material* edgeMaterial_;

		virtual const void resolveMeshes() = 0;
		void onStart() override;
		void onDestruction() override;
};

#endif 
