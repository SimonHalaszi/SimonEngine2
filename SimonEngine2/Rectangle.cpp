#include "Rectangle.hpp"

#include "Engine.hpp"

Rectangle::Rectangle(const EngineMath::Transform& transform, const EngineUtil::ColorRGB& color) {
	localTransform_ = transform;
	color_ = color;

    int updateSpeed = Engine::getInstance().getCurrentScene()->getUpdateSpeed();
    rotationSpeed_ = EngineMath::degreesToRadians(90.0f / updateSpeed);
}

void Rectangle::onStart() {
    static bool doThis = true;

    if (doThis) {
        doThis = false;
        addChild(std::make_unique<Rectangle>(
            EngineMath::Transform(
                EngineMath::Vector3(0.0f, 0.0f, -1.0f),
                EngineMath::Quaternion::identity(),
                EngineMath::Vector3(0.5f, 0.5f, 0.5f)
            ),
            EngineUtil::ColorRGB{ 1.0f, 0.5f, 0.5f }
        ));
        addChild(std::make_unique<Rectangle>(
            EngineMath::Transform(
                EngineMath::Vector3(0.0f, 0.0f, 1.0f),
                EngineMath::Quaternion::identity(),
                EngineMath::Vector3(0.25f, 0.25f, 0.5f)
            ),
            EngineUtil::ColorRGB{ 1.0f, 0.5f, 0.5f }
        ));
    }
}

void Rectangle::draw() {
    glDisable(GL_TEXTURE_2D);

    const float hx = 0.5f;
    const float hy = 0.5f;
    const float hz = 0.5f;

    glBegin(GL_QUADS);

    // Front face, red
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-hx, -hy, hz);
    glVertex3f(hx, -hy, hz);
    glVertex3f(hx, hy, hz);
    glVertex3f(-hx, hy, hz);

    // Back face, green
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(hx, -hy, -hz);
    glVertex3f(-hx, -hy, -hz);
    glVertex3f(-hx, hy, -hz);
    glVertex3f(hx, hy, -hz);

    // Left face, blue
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-hx, -hy, -hz);
    glVertex3f(-hx, -hy, hz);
    glVertex3f(-hx, hy, hz);
    glVertex3f(-hx, hy, -hz);

    // Right face, yellow
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3f(hx, -hy, hz);
    glVertex3f(hx, -hy, -hz);
    glVertex3f(hx, hy, -hz);
    glVertex3f(hx, hy, hz);

    // Top face, cyan
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex3f(-hx, hy, hz);
    glVertex3f(hx, hy, hz);
    glVertex3f(hx, hy, -hz);
    glVertex3f(-hx, hy, -hz);

    // Bottom face, magenta
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex3f(-hx, -hy, -hz);
    glVertex3f(hx, -hy, -hz);
    glVertex3f(hx, -hy, hz);
    glVertex3f(-hx, -hy, hz);

    glEnd();
}

void Rectangle::update() {
    rotationRadians_ += rotationSpeed_;

    setRotation(
        EngineMath::Quaternion::fromXYZ(
            rotationRadians_ * 0.75f,
            rotationRadians_,
            rotationRadians_ * 0.25f
        )
    );
}