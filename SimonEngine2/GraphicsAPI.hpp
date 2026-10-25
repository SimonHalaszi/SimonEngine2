#ifndef GRAPHICS_API_HPP
#define GRAPHICS_API_HPP

#ifdef _WIN32
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif

	#ifndef NOMINMAX
		#define NOMINMAX
	#endif

	#include <windows.h>
#endif

#include <glad/glad.h>
#include <GL/freeglut.h>

#endif