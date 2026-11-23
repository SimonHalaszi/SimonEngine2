# SimonEngine2

A lightweight 3D game engine built with C++20, OpenGL, FreeGLUT, GLAD, and FreeImage.

## Features

- Scene and object architecture
  - Scene initialization, updates, drawing, and cleanup
  - Hierarchical game objects with local and world transforms
  - Object lifecycle hooks for starting, updating, drawing, and destruction
- Rendering
  - Queued mesh submission and a pass-oriented renderer
  - Solid and wireframe drawing modes
  - Directional, point, and spot lights
  - Cubemaps, skyboxes, and textured materials
- Meshes and assets
  - Procedural primitive meshes and OBJ loading
  - Registries for meshes, materials, and textures
  - Albedo, normal, roughness, and metallic texture support
- Camera, input, and timing
  - Perspective and orthographic cameras
  - Keyboard and mouse input
  - Fixed-rate scene updates with capped or uncapped rendering

## Build

1. Open `SimonEngine2.sln` in Visual Studio 2022.
2. Select the `x64` platform and `Debug` or `Release` configuration.
3. Build and run the `SimonEngine2` project.

## Project layout

- `Assets/` — OBJ models and textures
- `Shaders/` — GLSL shader files
- `SimonEngine2/` — engine and example-scene source, Visual Studio project, and bundled dependencies
- `SimonEngine2/include/`, `SimonEngine2/lib/` — third-party headers and libraries

## Screenshot as of commit


