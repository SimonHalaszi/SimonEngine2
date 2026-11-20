#include "ArcballCamera.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "Engine.hpp"
#include "InputManager.hpp"

namespace {
    constexpr float rotationSpeed = 0.005f;
    constexpr float zoomSpeed = 0.01f;
    constexpr float panSpeed = 0.002f;
    constexpr float minDistance = 0.2f;
    constexpr float maxDistance = 500.0f;
    constexpr float pitchLimit = EngineMath::PI * 0.5f - 0.01f;
}

ArcballCamera::ArcballCamera(EngineMath::Vector3 target, float distance, float speed) : distance_(distance), speed_(speed) {
    if (distance < minDistance || distance > maxDistance) {
        throw std::invalid_argument("ArcballCameraMan: distance must be between 0.2 and 500");
    }

    speed_ = speed;
    setPosition(target);
    setRotation(EngineMath::Quaternion::fromXYZ(pitch_, yaw_, 0.0f));
}

void ArcballCamera::onStart() {
    Scene* scene = Engine::getInstance().getCurrentScene();
    if (!scene) {
        return;
    }

    scene->getCamera().setPerspective(90.0, 0.1, 1000.0);
    applyCamera();
}

void ArcballCamera::update() {
    const InputManager& input = InputManager::getInstance();
    const float dx = static_cast<float>(input.mouseDeltaX());
    const float dy = static_cast<float>(input.mouseDeltaY());
    const bool shiftDown = input.isSpecialKeyDown(mapSpecialKey(GLUT_KEY_SHIFT_L)) || input.isSpecialKeyDown(mapSpecialKey(GLUT_KEY_SHIFT_R));

    if (input.isMouseButtonDown(MOUSEBUTTON_LEFT)) {
        if (shiftDown) {
            const EngineMath::Quaternion rotation = getLocalTransform().rotation_;
            const EngineMath::Vector3 right = rotation.rotate(EngineMath::Vector3(1.0f, 0.0f, 0.0f));
            const EngineMath::Vector3 up = rotation.rotate(EngineMath::Vector3(0.0f, 1.0f, 0.0f));

            setPosition(getLocalTransform().position_ + (right * -dx + up * dy) * (distance_ * panSpeed));
        }
        else {
            yaw_ = std::remainder(yaw_ - dx * rotationSpeed, 2.0f * EngineMath::PI);
            pitch_ = std::clamp(pitch_ - dy * rotationSpeed, -pitchLimit, pitchLimit);

            setRotation(EngineMath::Quaternion::fromXYZ(pitch_, yaw_, 0.0f));
        }
    }

    if (input.isMouseButtonDown(MOUSEBUTTON_RIGHT)) {
        const float zoom = std::exp(std::clamp(dy * zoomSpeed, -10.0f, 10.0f));
        distance_ = std::clamp(distance_ * zoom, minDistance, maxDistance);
    }

    const float moveStep = speed_ * Engine::getInstance().getDeltaTime();
    const EngineMath::Quaternion rotation = getLocalTransform().rotation_;
    const EngineMath::Vector3 forward = rotation.rotate(EngineMath::Vector3(0.0f, 0.0f, -1.0f));
    const EngineMath::Vector3 right = rotation.rotate(EngineMath::Vector3(1.0f, 0.0f, 0.0f));

    EngineMath::Vector3 movement;

    if (input.isSpecialKeyDown(mapSpecialKey(GLUT_KEY_UP))) {
        movement += forward;
    }
    if (input.isSpecialKeyDown(mapSpecialKey(GLUT_KEY_DOWN))) {
        movement -= forward;
    }
    if (input.isSpecialKeyDown(mapSpecialKey(GLUT_KEY_LEFT))) {
        movement -= right;
    }
    if (input.isSpecialKeyDown(mapSpecialKey(GLUT_KEY_RIGHT))) {
        movement += right;
    }

    if (movement.lengthSquared() > 0.0f) {
        setPosition(getLocalTransform().position_ + movement.normalized() * moveStep);
    }

    applyCamera();
}

void ArcballCamera::applyCamera() {
    Scene* scene = Engine::getInstance().getCurrentScene();
    if (!scene) {
        throw std::logic_error("ArcballCameraMan::applyCamera: no active scene");
    }

    updateWorldTransform();

    EngineMath::Transform cameraTransform = getWorldTransform();
    cameraTransform.position_ += cameraTransform.rotation_.rotate(EngineMath::Vector3(0.0f, 0.0f, distance_));

    scene->getCamera().setTransform(cameraTransform);
}