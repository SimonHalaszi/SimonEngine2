#ifndef ENGINE_SHADER_HPP
#define ENGINE_SHADER_HPP

#include <string>

#include "GraphicsAPI.hpp"

/*
Shader loading and compilation was based on standard OpenGL patterns from the Khronos OpenGL Documentation,
as well as other references.
*/

namespace EngineShader {
	GLuint loadProgram(const std::string& vertexPath, const std::string& fragmentPath);
}

#endif 

