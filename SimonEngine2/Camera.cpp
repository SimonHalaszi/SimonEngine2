#include "Camera.hpp"

#include <algorithm>

void Camera::setPerspective(double fovy, double zNear, double zFar) {
	projectionMode_ = ProjectionMode::Perspective;
	perspectiveFovy_ = fovy;
	perspectiveNear_ = zNear;
	perspectiveFar_ = zFar;
}

void Camera::setOrthographic(double halfHeight, double zNear, double zFar) {
	projectionMode_ = ProjectionMode::Ortho;
	orthoHalfHeight_ = halfHeight;
	orthoNear_ = zNear;
	orthoFar_ = zFar;
}

void Camera::setProjectionMode(ProjectionMode mode) {
	projectionMode_ = mode;
}

Camera::ProjectionMode Camera::getProjectionMode() const {
	return projectionMode_;
}

void Camera::setPosition(const EngineMath::Vector3& position) {
	position_ = position;
}

void Camera::setTarget(const EngineMath::Vector3& target) {
	target_ = target;
}

void Camera::setZoom(double zoom) {
	zoom_ = std::max(0.1, zoom);
}

void Camera::zoomIn(double amount) {
	setZoom(zoom_ + amount);
}

void Camera::zoomOut(double amount) {
	setZoom(zoom_ - amount);
}

double Camera::getZoom() const {
	return zoom_;
}

void Camera::apply() const {
	applyProjection();
	applyView();
}

void Camera::applyProjection() const {
	const int width = glutGet(GLUT_WINDOW_WIDTH);
	const int height = glutGet(GLUT_WINDOW_HEIGHT);

	if (width <= 0 || height <= 0) {
		return;
	}

	const double aspectRatio = static_cast<double>(width) / static_cast<double>(height);

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (projectionMode_ == ProjectionMode::Perspective) {
		double zoomedFovy = perspectiveFovy_ / zoom_;
		if (zoomedFovy > 170.0) {
			zoomedFovy = 150;
		}

		gluPerspective(zoomedFovy, aspectRatio, perspectiveNear_, perspectiveFar_);
	}
	else {
		const double halfHeight = orthoHalfHeight_ / zoom_;
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