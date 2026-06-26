# MiniEngine

A small C++20 OpenGL engine built for learning and experimentation. It provides a forward renderer with instanced drawing, a deferred rendering path, and a simple scene graph with first-person camera controls.

## Features

- **Forward rendering** — Blinn-Phong lighting with diffuse textures, procedural skybox, and optional ground grid overlay
- **Deferred rendering** — G-buffer geometry pass followed by a fullscreen lighting pass (toggle at runtime)
- **Instanced rendering** — Large cube grids drawn in a single draw call for performance testing
- **Scene system** — Entities with transforms, materials, and mesh references
- **First-person camera** — Mouse look, WASD movement, scroll-to-zoom FOV, adjustable move speed
- **Runtime toggles** — Wireframe mode, grid visibility, VSync, cursor capture, and FPS logging to the console

## Requirements

- CMake 3.20 or newer
- A C++20 compiler (MSVC, GCC, or Clang)
- OpenGL 3.3 Core profile
- Network access on first configure (dependencies are fetched automatically)

## Building

Clone the repository and configure a build directory:

```bash
git clone https://github.com/leffloard/MiniEngine.git
cd MiniEngine
cmake -S . -B build
cmake --build build --config Release
```

On Windows with Visual Studio, you can open the generated solution in `build/` or build from the command line as shown above.

The first CMake run downloads [GLFW](https://github.com/glfw/glfw), [GLAD](https://github.com/Dav1dde/glad), [GLM](https://github.com/g-truc/glm), and `stb_image.h` into the build tree and `vendor/` respectively. Shaders are copied next to the executable automatically.

## Running

Run the executable from its output directory so the `Shaders/` folder is found:

```bash
# Windows (Release)
./build/Release/MiniEngine.exe

# Or from Visual Studio — set MiniEngine as startup project and run
```

If shaders are not found next to the binary, the application falls back to loading them from `src/Shaders/` relative to the working directory.

## Controls

| Key | Action |
|-----|--------|
| W / A / S / D | Move forward / left / back / right |
| Space / Ctrl | Move up / down |
| Mouse | Look around (when cursor is captured) |
| Scroll | Adjust field of view |
| Shift + Scroll | Adjust move speed |
| Shift (held) | Sprint |
| F1 | Toggle VSync |
| F2 | Toggle ground grid |
| F3 | Toggle wireframe |
| F4 | Toggle cursor capture |
| F5 | Toggle instanced cube grid |
| F9 | Toggle deferred / forward rendering |
| Escape | Quit |

FPS is printed to the console once per second.

## Project Structure

```
MiniEngine/
├── CMakeLists.txt          Build configuration and dependency fetching
├── src/
│   ├── main.cpp            Application entry point and main loop
│   ├── Core/               Time, input, and asset helpers
│   ├── Scene/              Camera, transforms, entities, and scene setup
│   ├── Gfx/                Shaders, meshes, textures, renderers
│   └── Shaders/            GLSL vertex and fragment shaders
└── vendor/                 Third-party headers (stb_image.h, auto-downloaded)
```

## Rendering Paths

**Forward (default)** — The scene is drawn directly with per-object lighting in a single pass. Instanced cubes use a dedicated instanced vertex shader.

**Deferred (F9)** — Geometry is written to a G-buffer (position, normal, albedo, material). A fullscreen pass reconstructs lighting from the buffer. The skybox is still rendered in forward mode after the lighting pass.

## License

This project is provided as-is for educational purposes. Third-party libraries fetched by CMake retain their own licenses.
