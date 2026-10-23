#include "StudioLighting.hpp"

#include <GL/freeglut.h>

namespace StudioLighting {

	namespace {
		// Fixed-function lights want float[4]. For DIRECTIONAL lights the 4th component
		// is 0 and xyz is the direction the light comes FROM (it doesn't need to be unit
		// length; OpenGL normalizes directional light vectors itself).
		void setDirectional(GLenum light, float x, float y, float z) {
			const GLfloat position[4] = { x, y, z, 0.0f };
			glLightfv(light, GL_POSITION, position);
		}

		void setColor(GLenum light, GLenum which, const GamerTheme::Rgb& c, float intensity) {
			const GLfloat color[4] = { c.r * intensity, c.g * intensity, c.b * intensity, 1.0f };
			glLightfv(light, which, color);
		}
	}

	void apply(const GamerTheme::Theme& palette) {
		// -------------------------------------------------------------- positions
		// glLightfv(GL_POSITION) transforms the position by whatever modelview matrix is
		// current. Loading identity first means "these directions are in camera space",
		// which glues the whole rig to the camera.
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		setDirectional(GL_LIGHT0, -0.45f, 0.80f, 0.65f); // KEY: up-left, toward the viewer
		setDirectional(GL_LIGHT1, -1.00f, 0.30f, -0.35f); // RIM: left, slightly behind
		setDirectional(GL_LIGHT2, 1.00f, 0.15f, -0.30f); // KICK: right, slightly behind

		glPopMatrix();

		// -------------------------------------------------------------- colors
		const GamerTheme::Rgb warmWhite = { 1.0f, 0.96f, 0.90f };
		const GamerTheme::Rgb black = { 0.0f, 0.0f, 0.0f };

		setColor(GL_LIGHT0, GL_DIFFUSE, warmWhite, 0.85f);
		setColor(GL_LIGHT0, GL_SPECULAR, warmWhite, 0.65f);
		setColor(GL_LIGHT0, GL_AMBIENT, black, 0.0f);

		// The rims tint everything with the theme colors, and give a matching specular
		// glint so glossy surfaces catch a neon highlight along their edges
		setColor(GL_LIGHT1, GL_DIFFUSE, palette.accent, 0.95f);
		setColor(GL_LIGHT1, GL_SPECULAR, palette.accent, 0.55f);
		setColor(GL_LIGHT1, GL_AMBIENT, black, 0.0f);

		setColor(GL_LIGHT2, GL_DIFFUSE, palette.accent2, 0.80f);
		setColor(GL_LIGHT2, GL_SPECULAR, palette.accent2, 0.45f);
		setColor(GL_LIGHT2, GL_AMBIENT, black, 0.0f);

		// Global ambient, nudged toward the sky color so shadows aren't dead black
		const GLfloat ambient[4] = {
			0.22f + palette.skyHorizon.r * 0.20f,
			0.22f + palette.skyHorizon.g * 0.20f,
			0.24f + palette.skyHorizon.b * 0.20f,
			1.0f
		};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

		// Two-sided lighting: if a mesh happens to be wound inside-out, the back faces
		// still get lit correctly instead of rendering pitch black. Cheap insurance.
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

		// -------------------------------------------------------------- material
		// COLOR_MATERIAL = "glColor3f() sets the material's ambient + diffuse color".
		// That's the whole trick that lets the existing primitives keep calling glColor3f
		// exactly like before, and still get shaded.
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
		glEnable(GL_COLOR_MATERIAL);

		const GLfloat specular[4] = { 0.55f, 0.55f, 0.55f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 42.0f);

		// Objects get non-uniform scales (the torso is 1 x 1.5 x 0.5), which squashes
		// normals out of unit length. GL_NORMALIZE re-normalizes them after the modelview
		// transform. Without it the lighting gets weirdly bright/dark per object.
		glEnable(GL_NORMALIZE);
		glShadeModel(GL_SMOOTH);

		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
		glEnable(GL_LIGHTING);
	}

	void disable() {
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
		glDisable(GL_COLOR_MATERIAL);
	}
}
