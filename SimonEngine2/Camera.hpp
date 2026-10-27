#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "EngineMath.hpp"

class Camera {
	public:
		enum class ProjectionMode { Ortho, Perspective };

		void setPerspective(double fovy, double zNear, double zFar);
		void setOrthographic(double halfHeight, double zNear, double zFar);
		
		void setProjectionMode(ProjectionMode mode);
		ProjectionMode getProjectionMode() const;

		void setFOV(double fov);

		void setPosition(const EngineMath::Vector3& position);
		void setTarget(const EngineMath::Vector3& target);
		void setTransform(const EngineMath::Transform& transform);

		EngineMath::Matrix4 getViewMatrix() const;
		EngineMath::Matrix4 getProjectionMatrix(float aspectRatio) const;
		EngineMath::Matrix4 getViewProjectionMatrix(float aspectRatio) const;

		void pivotAroundTarget(const EngineMath::Quaternion& rotation);
		void pivotAroundTarget(const EngineMath::Vector3& axis, float radians);

		void zoomIn(double amount);
		void zoomOut(double amount);

		double getZoom() const;

	private:
		ProjectionMode projectionMode_ = ProjectionMode::Perspective;

		double perspectiveFovy_ = 90.0;
		double perspectiveNear_ = 0.1;
		double perspectiveFar_ = 100.0;
		double perspectiveZoomFov_ = perspectiveFovy_;

		double orthoHalfHeight_ = 2.3;
		double orthoNear_ = 0.1;
		double orthoFar_ = 100.0;
		double orthoZoomScale_ = 1.0;

		EngineMath::Vector3 position_ = { 0.0f, 0.0f, 0.0f };
		EngineMath::Vector3 target_ = { 0.0f, 0.0f, -1.0f };
		EngineMath::Vector3 up_ = { 0.0f, 1.0f, 0.0f };
};

#endif