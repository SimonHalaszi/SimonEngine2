#include "Camera.hpp"
#include <algorithm>

namespace {
	constexpr double minPerspectiveFov = 1.0;
	constexpr double maxPerspectiveFov = 170.0;

	constexpr double minOrthoScale = 0.01;
	constexpr double maxOrthoScale = 100.0;
}


void Camera::setPerspective(double fovy, double zNear, double zFar) {
	projectionMode_ = ProjectionMode::Perspective;

	perspectiveFovy_ = std::clamp(fovy, minPerspectiveFov, maxPerspectiveFov);

	perspectiveNear_ = zNear;
	perspectiveFar_ = zFar;
	perspectiveZoomFov_ = perspectiveFovy_;
}

void Camera::setOrthographic(double halfHeight, double zNear, double zFar) {
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

void Camera::updateZoomFromPosition() {
	zoomPosition_ = std::clamp(zoomPosition_, -1.0, 1.0);

	if (zoomPosition_ >= 0.0) {
		perspectiveZoomFov_ = perspectiveFovy_ - zoomPosition_ * (perspectiveFovy_ - minPerspectiveFov);
	}
	else {
		const double zoomOutPosition = -zoomPosition_;
		perspectiveZoomFov_ = perspectiveFovy_ + zoomOutPosition * (maxPerspectiveFov - perspectiveFovy_);
	}

	perspectiveZoomFov_ = std::clamp(perspectiveZoomFov_, minPerspectiveFov, maxPerspectiveFov);

	const double baseTan = std::tan(EngineMath::degreesToRadians(perspectiveFovy_ * 0.5));

	const double zoomedTan = std::tan(EngineMath::degreesToRadians(perspectiveZoomFov_ * 0.5));

	orthoZoomScale_ = baseTan > 0.0 ? zoomedTan / baseTan : 1.0;

	orthoZoomScale_ = std::clamp(orthoZoomScale_, minOrthoScale, maxOrthoScale);
}

void Camera::setZoomFromPerspectiveFov(double fov) {
	fov = std::clamp(fov, minPerspectiveFov, maxPerspectiveFov);

	if (fov <= perspectiveFovy_) {
		const double range = perspectiveFovy_ - minPerspectiveFov;
		zoomPosition_ = range > 0.0 ? (perspectiveFovy_ - fov) / range : 1.0;
	}
	else {
		const double range = maxPerspectiveFov - perspectiveFovy_;
		zoomPosition_ = range > 0.0 ? -(fov - perspectiveFovy_) / range : -1.0;
	}

	updateZoomFromPosition();
}

void Camera::setZoomFromOrthoScale(double scale) {
	scale = std::clamp(scale, minOrthoScale, maxOrthoScale);

	if (scale <= 1.0) {
		zoomPosition_ = (1.0 - scale) / (1.0 - minOrthoScale);
	}
	else {
		zoomPosition_ = -(scale - 1.0) / (maxOrthoScale - 1.0);
	}

	updateZoomFromPosition();
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

void Camera::apply() const {
	applyProjection();
	applyView();
}

void Camera::applyProjection() const {
	const int width = glutGet(GLUT_WINDOW_WIDTH);
	const int height = glutGet(GLUT_WINDOW_HEIGHT);

	if (width <= 0 || height <= 0) return;

	const double aspectRatio = static_cast<double>(width) / static_cast<double>(height);

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (projectionMode_ == ProjectionMode::Perspective) {
		gluPerspective(perspectiveZoomFov_, aspectRatio, perspectiveNear_, perspectiveFar_);
	}
	else {
		const double halfHeight = orthoHalfHeight_ * orthoZoomScale_;
		const double halfWidth = halfHeight * aspectRatio;

		glOrtho(-halfWidth, halfWidth, -halfHeight, halfHeight, orthoNear_, orthoFar_);
	}
}

void Camera::applyView() const {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(
		position_.x_, position_.y_, position_.z_,
		target_.x_, target_.y_, target_.z_,
		up_.x_, up_.y_, up_.z_
	);
}