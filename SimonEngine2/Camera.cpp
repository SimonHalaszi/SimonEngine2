#include "Camera.hpp"
#include <algorithm>

namespace {
	constexpr double minPerspectiveFov = 1.0;
	constexpr double maxPerspectiveFov = 170.0;

	constexpr double minOrthoScale = 0.01;
	constexpr double maxOrthoScale = 100.0;
}

void Camera::setPerspective(double fovy, double zNear, double zFar) {
	if (!(zNear > 0.0 && zFar > zNear)) {
		throw std::invalid_argument("Camera::setPerspective: expected 0 < zNear < zFar");
	}

	projectionMode_ = ProjectionMode::Perspective;

	perspectiveFovy_ = std::clamp(fovy, minPerspectiveFov, maxPerspectiveFov);

	perspectiveNear_ = zNear;
	perspectiveFar_ = zFar;
	perspectiveZoomFov_ = perspectiveFovy_;
}

void Camera::setOrthographic(double halfHeight, double zNear, double zFar) {
	if (!(halfHeight > 0.0)) {
		throw std::invalid_argument("Camera::setOrthographic: halfHeight must be positive");
	}

	if (!(zNear > 0.0 && zFar > zNear)) {
		throw std::invalid_argument("Camera::setOrthographic: expected 0 < zNear < zFar");
	}
	
	projectionMode_ = ProjectionMode::Ortho;
	orthoHalfHeight_ = halfHeight;
	orthoNear_ = zNear;
	orthoFar_ = zFar;
	orthoZoomScale_ = 1.0;
}

void Camera::setProjectionMode(ProjectionMode mode) {
	projectionMode_ = mode;
}

Camera::ProjectionMode Camera::getProjectionMode() const {
	return projectionMode_;
}

void Camera::setFOV(double fov) {
	if (projectionMode_ == ProjectionMode::Ortho) {
		return;
	}
	
	perspectiveFovy_ = std::clamp(fov, minPerspectiveFov, maxPerspectiveFov);

	perspectiveZoomFov_ = perspectiveFovy_;
}

void Camera::setPosition(const EngineMath::Vector3& position) {
	position_ = position;
}

void Camera::setTarget(const EngineMath::Vector3& target) {
	target_ = target;
}

void Camera::setTransform(const EngineMath::Transform& worldTransform) {
	position_ = worldTransform.position_;

	const EngineMath::Vector3 forward = worldTransform.rotation_.rotate(EngineMath::Vector3(0.0f, 0.0f, -1.0f));

	target_ = position_ + forward;

	up_ = worldTransform.rotation_.rotate(EngineMath::Vector3(0.0f, 1.0f, 0.0f));
}

void Camera::pivotAroundTarget(const EngineMath::Quaternion& rotation) {
	EngineMath::Quaternion q = rotation.normalized();

	position_ = target_ + q.rotate(position_ - target_);
	up_ = q.rotate(up_);
}

void Camera::pivotAroundTarget(const EngineMath::Vector3& axis, float radians) {
	pivotAroundTarget(EngineMath::Quaternion::fromAxisAngle(axis, radians));
}

void Camera::zoomIn(double amount) {
	if (projectionMode_ == ProjectionMode::Perspective) {
		perspectiveZoomFov_ = std::clamp(perspectiveZoomFov_ - amount, minPerspectiveFov, maxPerspectiveFov);
	}
	else {
		orthoZoomScale_ = std::clamp(orthoZoomScale_ - amount, minOrthoScale, maxOrthoScale);
	}
}

void Camera::zoomOut(double amount) {
	if (projectionMode_ == ProjectionMode::Perspective) {
		perspectiveZoomFov_ = std::clamp(perspectiveZoomFov_ + amount, minPerspectiveFov, maxPerspectiveFov);
	}
	else {
		orthoZoomScale_ = std::clamp(orthoZoomScale_ + amount, minOrthoScale, maxOrthoScale);
	}
}

double Camera::getZoom() const {
	if (projectionMode_ == ProjectionMode::Perspective) {
		return perspectiveFovy_ / perspectiveZoomFov_;
	}
	return 1.0 / orthoZoomScale_;
}

EngineMath::Matrix4 Camera::getViewMatrix() const {
	return EngineMath::Matrix4::lookAt(position_, target_, up_);
}

EngineMath::Matrix4 Camera::getProjectionMatrix(float aspectRatio) const {
	if (projectionMode_ == ProjectionMode::Ortho) {
		const float halfHeight = static_cast<float>(orthoHalfHeight_ * orthoZoomScale_);

		const float halfWidth = halfHeight * aspectRatio;

		return EngineMath::Matrix4::orthographic(
			-halfWidth,
			halfWidth,
			-halfHeight,
			halfHeight,
			static_cast<float>(orthoNear_),
			static_cast<float>(orthoFar_)
		);
	}
	else {
		return EngineMath::Matrix4::perspective(
			EngineMath::degreesToRadians(static_cast<float>(perspectiveZoomFov_)),
			aspectRatio,
			static_cast<float>(perspectiveNear_),
			static_cast<float>(perspectiveFar_)
		);
	}
}

EngineMath::Matrix4 Camera::getViewProjectionMatrix(float aspectRatio) const {
	return getProjectionMatrix(aspectRatio) * getViewMatrix();
}