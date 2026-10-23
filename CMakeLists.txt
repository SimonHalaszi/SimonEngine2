# ==================================================================================
#   SimonEngine2 :: CHUCK SPECIAL EDITION :: CMake build
#
#   The Visual Studio solution (SimonEngine2.sln) is still the main way to build on
#   Windows. This file exists so the engine also builds on Linux (and anywhere else
#   with freeglut), in VS Code, in CLion, and in CI.
#
#     cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
#     cmake --build build -j
#     cd SimonEngine2 && ../build/SimonEngine2     # run from here so ../Assets resolves
# ==================================================================================

cmake_minimum_required(VERSION 3.16)

project(SimonEngine2
	VERSION 2.0.69
	DESCRIPTION "SimonEngine2, but better. CHUCK SPECIAL EDITION."
	LANGUAGES C CXX
)

# Every .cpp in the engine folder. (glad.c is skipped on purpose: it ships with the
# Windows project for future modern-GL work, but nothing calls into it yet.)
file(GLOB SE2_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/SimonEngine2/*.cpp")

add_executable(SimonEngine2 ${SE2_SOURCES})

# Camera.cpp needs std::clamp (C++17). Everything else is written to compile as C++14,
# which is what MSVC uses by default for the rest of the .vcxproj.
target_compile_features(SimonEngine2 PRIVATE cxx_std_17)
set_target_properties(SimonEngine2 PROPERTIES CXX_EXTENSIONS OFF)

if (WIN32)
	# Windows: use the freeglut that ships in the repo (static, MSVC-built).
	# freeglut_std.h auto-links freeglut_static(d).lib, opengl32, glu32, gdi32, winmm and
	# user32 through #pragma comment(lib), so we only have to say where the .lib lives.
	target_compile_definitions(SimonEngine2 PRIVATE FREEGLUT_STATIC)
	target_include_directories(SimonEngine2 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/SimonEngine2/include")
	target_link_directories(SimonEngine2 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/SimonEngine2/lib")

	# F5 in Visual Studio runs from the engine folder, so ../Assets/teapot.obj is found
	set_target_properties(SimonEngine2 PROPERTIES
		VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/SimonEngine2"
	)
else()
	# Linux / BSD / macOS-with-Homebrew-freeglut: use the system packages
	#   Debian/Ubuntu:  sudo apt install freeglut3-dev libglu1-mesa-dev
	#   Fedora:         sudo dnf install freeglut-devel mesa-libGLU-devel
	#   Arch:           sudo pacman -S freeglut glu
	set(OpenGL_GL_PREFERENCE GLVND)
	find_package(OpenGL REQUIRED)
	find_package(GLUT REQUIRED)
	target_link_libraries(SimonEngine2 PRIVATE GLUT::GLUT OpenGL::GL OpenGL::GLU)
endif()

if (MSVC)
	target_compile_options(SimonEngine2 PRIVATE /W3 /permissive-)
else()
	target_compile_options(SimonEngine2 PRIVATE -Wall -Wextra -Wno-unused-parameter)
endif()
