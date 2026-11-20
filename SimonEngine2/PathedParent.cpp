#include "PathedParent.hpp"

#include <cmath>
#include <utility>

#include "Engine.hpp"
#include "MeshRegistry.hpp"
#include "MaterialRegistry.hpp"
#include "Renderer.hpp"

namespace {
    constexpr float directionEpsilon = 1e-8f;
    constexpr float pointMarkerDiameter = 0.2f;

    EngineMath::Quaternion rotationFromTo(const EngineMath::Vector3& from, const EngineMath::Vector3& to) {
        const EngineMath::Vector3 axis = from.cross(to);
        float dot = from.dot(to);
        dot = dot > 1.0f ? 1.0f : dot;
        dot = dot < -1.0f ? -1.0f : dot;

        if (axis.lengthSquared() > directionEpsilon) {
            return EngineMath::Quaternion::fromAxisAngle(axis, std::acos(dot));
        }

        if (dot >= 0.0f) {
            return EngineMath::Quaternion::identity();
        }

        const EngineMath::Vector3 fallback = std::abs(from.x_) < 0.9f
            ? EngineMath::Vector3(1.0f, 0.0f, 0.0f)
            : EngineMath::Vector3(0.0f, 1.0f, 0.0f);

        return EngineMath::Quaternion::fromAxisAngle(from.cross(fallback), EngineMath::PI);
    }
}

PathedParent::PathedParent(std::unique_ptr<Object> child, std::vector<EngineMath::Vector3> positions, float speed, const EngineRender::Material* pathMaterial)
    : isMoving_(true), isPathDrawing_(false), drawPathAsSolid_(true), positions_(std::move(positions)), speed_(speed) {
    if (pathMaterial == nullptr) {
        pathMaterial = MaterialRegistry::getInstance().getOrCreate("null", EngineRender::Material());
    }

    pathMaterial_ = pathMaterial;
    edgeMaterial_ = MaterialRegistry::getInstance().getOrCreate("Edge", EngineRender::Material());
    addChild(std::move(child));
    resetPosition();
}

void PathedParent::onStart() {

}

void PathedParent::pushBackPosition(EngineMath::Vector3 position) {
    positions_.push_back(position);
    resetPosition();
}

void PathedParent::setPositionsVector(std::vector<EngineMath::Vector3> positions) {
    positions_ = std::move(positions);
    resetPosition();
}

void PathedParent::resetPosition() {
    segmentStartIndex_ = 0;
    distanceAlongSegment_ = 0.0f;

    if (positions_.empty()) {
        return;
    }

    setPosition(positions_.front());

    const EngineMath::Vector3 forward(0.0f, 0.0f, -1.0f);
    for (std::size_t i = 1; i < positions_.size(); ++i) {
        const EngineMath::Vector3 direction = positions_[i] - positions_.front();
        if (direction.length() > directionEpsilon) {
            setRotation(rotationFromTo(forward, direction.normalized()));
            return;
        }
    }
}

void PathedParent::update() {
    // To keep consistent behavior with primitives
    if (InputManager::getInstance().isPressed('1')) {
        drawPathAsSolid_ = false;
    }
    if (InputManager::getInstance().isPressed('2')) {
        drawPathAsSolid_ = true;
    }
    if (InputManager::getInstance().isPressed('s')) {
        toggleMovement();
    }
    if (InputManager::getInstance().isPressed('r')) {
        resetPosition();
    }
    if (InputManager::getInstance().isPressed('4')) {
        togglePathDrawing();
    }

    if (!isMoving_ || speed_ <= 0.0f || positions_.size() < 2) {
        return;
    }

    float remainingDistance = speed_ * Engine::getInstance().getDeltaTime();
    if (remainingDistance <= 0.0f) {
        return;
    }

    std::size_t consecutiveZeroLengthSegments = 0;

    while (remainingDistance > 0.0f) {
        const std::size_t nextIndex = (segmentStartIndex_ + 1) % positions_.size();
        const EngineMath::Vector3 segment = positions_[nextIndex] - positions_[segmentStartIndex_];
        const float segmentLength = segment.length();

        if (segmentLength <= directionEpsilon) {
            segmentStartIndex_ = nextIndex;
            distanceAlongSegment_ = 0.0f;

            ++consecutiveZeroLengthSegments;
            if (consecutiveZeroLengthSegments >= positions_.size()) {
                break;
            }

            continue;
        }

        consecutiveZeroLengthSegments = 0;

        const float distanceToSegmentEnd = segmentLength - distanceAlongSegment_;
        if (remainingDistance < distanceToSegmentEnd) {
            distanceAlongSegment_ += remainingDistance;
            remainingDistance = 0.0f;
        }
        else {
            remainingDistance -= distanceToSegmentEnd;
            segmentStartIndex_ = nextIndex;
            distanceAlongSegment_ = 0.0f;
        }
    }

    const std::size_t nextIndex = (segmentStartIndex_ + 1) % positions_.size();
    const EngineMath::Vector3 segment = positions_[nextIndex] - positions_[segmentStartIndex_];
    const float segmentLength = segment.length();

    EngineMath::Vector3 position = positions_[segmentStartIndex_];
    if (segmentLength > directionEpsilon) {
        position += segment * (distanceAlongSegment_ / segmentLength);
    }
    setPosition(position);

    const EngineMath::Vector3 forward(0.0f, 0.0f, -1.0f);
    for (std::size_t offset = 0; offset < positions_.size(); ++offset) {
        const std::size_t startIndex = (segmentStartIndex_ + offset) % positions_.size();
        const std::size_t endIndex = (startIndex + 1) % positions_.size();
        const EngineMath::Vector3 direction = positions_[endIndex] - positions_[startIndex];

        if (direction.length() > directionEpsilon) {
            setRotation(rotationFromTo(forward, direction.normalized()));
            break;
        }
    }
}

void PathedParent::draw() const {
    if (positions_.empty() || !isPathDrawing_) {
        return;
    }

    const EngineMath::Matrix4 pathSpaceMatrix = parent_ ? parent_->getWorldTransform().toMatrix() : EngineMath::Matrix4::identity();

    // Drawing edges betwwen each position pair
    if (positions_.size() >= 2) {
        const EngineRender::GPUMesh* edgeMesh = MeshRegistry::getInstance().getGPUEdge();
        if (edgeMesh) {
            const EngineMath::Vector3 edgeForward(1.0f, 0.0f, 0.0f);
            const std::size_t segmentCount = positions_.size() == 2 ? 1 : positions_.size();

            for (std::size_t i = 0; i < segmentCount; ++i) {
                const EngineMath::Vector3& start = positions_[i];
                const EngineMath::Vector3& end = positions_[(i + 1) % positions_.size()];
                const EngineMath::Vector3 segment = end - start;
                const float length = segment.length();

                if (length <= directionEpsilon) {
                    continue;
                }

                const EngineMath::Transform segmentTransform(
                    (start + end) * 0.5f,
                    rotationFromTo(edgeForward, segment * (1.0f / length)),
                    EngineMath::Vector3(length, 1.0f, 1.0f)
                );

                // To keep consistent behavior with primitives
                const EngineRender::Material* activeMaterial = drawPathAsSolid_ ? pathMaterial_ : edgeMaterial_;

                Renderer::getInstance().drawMesh(*edgeMesh, *activeMaterial, pathSpaceMatrix * segmentTransform.toMatrix(), Renderer::DrawMode::Edges);
            }
        }
    }

    // Drawing points for each position
    if (positions_.size() >= 2) {
        const EngineRender::GPUMesh* pointMesh = MeshRegistry::getInstance().getGPUEllipsoid(8, 8);
        if (pointMesh) {
            for (const EngineMath::Vector3& position : positions_) {
                const EngineMath::Transform pointTransform(
                    position,
                    EngineMath::Quaternion::identity(),
                    EngineMath::Vector3(pointMarkerDiameter, pointMarkerDiameter, pointMarkerDiameter)
                );

                // To keep consistent behavior with primitives
                Renderer::DrawMode drawMode = Renderer::DrawMode::Solid;
                if (!drawPathAsSolid_) {
                    drawMode = Renderer::DrawMode::Edges;
                }

                const EngineRender::Material* activeMaterial = (drawMode == Renderer::DrawMode::Edges) ? edgeMaterial_ : pathMaterial_;

                Renderer::getInstance().drawMesh(*pointMesh, *activeMaterial, pathSpaceMatrix * pointTransform.toMatrix(), drawMode);
            }
        }
    }
}