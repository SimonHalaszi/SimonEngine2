#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <GL/glut.h>
#include <GL/freeglut.h>

#include "WindowConstants.hpp"
#include "EngineMath.hpp"

class Camera {
	public:
		enum class ProjectionMode { Ortho, Perspective };

		void setPerspective(double, double, double);
		void setOrthographic(double, double, double);
		
		void setProjectionMode(ProjectionMode mode);
		ProjectionMode getProjectionMode() const;

		void setPosition(const EngineMath::Vector3& position);
		void setTarget(const EngineMath::Vector3& target);
		void setZoom(double zoom);

		void zoomIn(double amount);
		void zoomOut(double amount);

		double getZoom() const;

		void apply() const;

	private:
		void applyProjection() const;
		void applyView() const;

		ProjectionMode projectionMode_ = ProjectionMode::Perspective;

		double perspectiveFovy_ = 60.0;
		double perspectiveNear_ = 0.1;
		double perspectiveFar_ = 100.0;

		double orthoHalfHeight_ = 2.3;
		double orthoNear_ = 0.1;
		double orthoFar_ = 100.0;

		double zoom_ = 1.0;

		EngineMath::Vector3 position_ = { 0.0f, 0.0f, 0.0f };
		EngineMath::Vector3 target_ = { 0.0f, 0.0f, -1.0f };
		EngineMath::Vector3 up_ = { 0.0f, 1.0f, 0.0f };
};

#endif