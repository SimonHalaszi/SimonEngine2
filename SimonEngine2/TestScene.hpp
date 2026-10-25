#ifndef TEST_SCENE_HPP
#define TEST_SCENE_HPP

#include "GraphicsAPI.hpp"

// Needed C++ Includes
#include <string>
#include <memory>
#include <iostream>

// Scene Interface
#include "Scene.hpp"

// Other SimonEngine includes
#include "InputManager.hpp"
#include "EngineUtil.hpp"
#include "MeshRegistry.hpp"
#include "GPUMesh.hpp"
#include "GPUMeshUpload.hpp"
#include "EngineShader.hpp"
#include "Renderer.hpp"

class TestScene : public Scene {
public:
	TestScene() : Scene(244, 244, 10) {}

	~TestScene() {}

	virtual void init() override final;

	virtual void draw() const override final;

	virtual void update() override final;

	virtual void deInit() override final;

private:
	const GPUMesh* gpuMesh_ = nullptr;
	EngineUtil::Material material_;

	GLuint colorProgram_ = 0;
	GLint uViewProjectionLoc_ = -1;
	GLint uModelLoc_ = -1;
	GLint uColorLoc_ = -1;
};

#endif