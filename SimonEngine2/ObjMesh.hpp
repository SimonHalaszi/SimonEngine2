#ifndef OBJ_MESH_HPP
#define OBJ_MESH_HPP

#include "Primitive.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"

#include <string>

class ObjMesh : public Primitive {
public:
	ObjMesh() = default;
	ObjMesh(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color, const std::string& filepath);

private:
	const EngineUtil::Mesh* mesh_ = nullptr;
	void drawSolid() const override;
	void drawEdges() const override;
};

#endif 