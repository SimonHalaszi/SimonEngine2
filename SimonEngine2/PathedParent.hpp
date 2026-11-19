#ifndef PATHED_PARENT_HPP
#define PATHED_PARENT_HPP

#include <cstddef>
#include <memory>
#include <vector>

#include "EngineRender.hpp"
#include "Object.hpp"

class PathedParent : public Object {
    public:
        PathedParent(std::unique_ptr<Object> child, std::vector<EngineMath::Vector3> positions, float speed, const EngineRender::Material& pathMaterial = {});

        void pushBackPosition(EngineMath::Vector3 position);
        void setPositionsVector(std::vector<EngineMath::Vector3> positions);
        void resetPosition();
        void toggleMovement() { isMoving_ = !isMoving_; }
        void togglePathDrawing() { isPathDrawing_ = !isPathDrawing_; }

    protected:
        void onStart() override;
        void update() override;
        void draw() const override;

    private:
        std::vector<EngineMath::Vector3> positions_;
        float speed_ = 0.0f;
        EngineRender::Material pathMaterial_;
        std::size_t segmentStartIndex_ = 0;
        float distanceAlongSegment_ = 0.0f;
        bool isMoving_;
        bool isPathDrawing_;
};

#endif