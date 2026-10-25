#include "EngineShader.hpp"

namespace {
	std::string readFile(const std::string& path) {
		std::ifstream file(path);

		if (!file) {
			throw std::runtime_error("Cannot open shader file: " + path);
		}

		std::ostringstream contents;
		contents << file.rdbuf();

		if (file.bad()) {
			throw std::runtime_error("Cannot read shader file: " + path);
		}

		return contents.str();
	}

	GLuint compileShader(GLenum type, const std::string& source, const std::string& path) {
		GLuint shader = glCreateShader(type);

		if (shader == 0) {
			throw std::runtime_error("Cannot create shader: " + path);
		}

		const char* sourceText = source.c_str();

		glShaderSource(shader, 1, &sourceText, nullptr);
		glCompileShader(shader);

		GLint success = GL_FALSE;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (success == GL_TRUE) {
			return shader;
		}

		// Useful log prints for if this fails
		GLint logLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

		std::string log(logLength > 0 ? logLength : 1, '\0');
		glGetShaderInfoLog(shader, logLength, nullptr, &log[0]);

		glDeleteShader(shader);

		throw std::runtime_error("Shader compilation failed: " + path + "\n" + log);
	}
}


GLuint EngineShader::loadProgram(const std::string& vertexPath, const std::string& fragmentPath) {
	const std::string vertexSource = readFile(vertexPath);
	const std::string fragmentSource = readFile(fragmentPath);

	GLuint vertex = compileShader(GL_VERTEX_SHADER, vertexSource, vertexPath);

	GLuint fragment = 0;
	GLuint program = 0;

	try {
		fragment = compileShader(GL_FRAGMENT_SHADER, fragmentSource, fragmentPath);

		program = glCreateProgram();

		if (program == 0) {
			throw std::runtime_error("Cannot create shader program");
		}

		glAttachShader(program, vertex);
		glAttachShader(program, fragment);
		glLinkProgram(program);

		GLint success = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &success);

		if (success != GL_TRUE) {
			GLint logLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

			std::string log(logLength > 0 ? logLength : 1, '\0');
			glGetShaderInfoLog(program, logLength, nullptr, &log[0]);

			throw std::runtime_error("Shader linking failed: " + vertexPath + "\n" + log);
		}
	}
	catch (...) {
		if (program != 0) {
			glDeleteProgram(program);
		}

		if (fragment != 0) {
			glDeleteShader(fragment);
		}

		glDeleteShader(vertex);
		throw;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return program;
}