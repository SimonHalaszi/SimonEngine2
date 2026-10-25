#include "ProperPrim.hpp"

ProperPrim::ProperPrim(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color, const std::string& vertPath, const std::string& fragPath) {
    localTransform_ = localTransform;
    material_.color_ = color;
    material_.shader_ = Renderer::getInstance().getOrLoadShader(vertPath, fragPath);
    mesh_ = MeshRegistry::getInstance().getGPUSphere(16, 16);
}

#include "Engine.hpp"

void ProperPrim::draw() {
    Camera& camera = Engine::getInstance().getCurrentScene()->getCamera();
    Renderer::getInstance().drawMesh(*mesh_, material_, localTransform_.toMatrix(), camera.getViewProjectionMatrix(1.0f));
}

void ProperPrim::update() {

}