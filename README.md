# Vectra

Vectra is a bespoke physics engine written in C++ from scratch. It is designed to be a simple and easy-to-use physics engine for simulations and games. Still under development, this is a glimpse at the current feature set.

## Features

- **Rigid Body Dynamics**: Simulates the motion of rigid bodies in 2D and 3D space.
- **Force Generators**: Includes a variety of force generators to simulate different physical phenomena:
  - **Gravity**: Simple and Newtonian gravity implementations.
  - **Springs**: Anchored and bungee spring forces.
- **Rendering**:
  - **OpenGL**: Uses modern OpenGL for rendering.
  - **Shaders**: Supports custom shaders for different materials and lighting effects (e.g., Phong shading).
  - **Models**: Loads 3D models using Assimp.
  - **Skybox**: Renders a skybox for immersive environments.
- **Component-Based Architecture**: Uses a modular and extensible component-based design with GameObjects and Scenes.

## Getting Started

### Prerequisites

- C++17 compiler
- CMake 4.0 or higher
- OpenGL
- GLM
- GLFW
- Assimp

### Building

1. **Clone the repository**:
   ```bash
   git clone https://github.com/your-username/vectra.git
   cd vectra
   ```

2. **Update submodules**:
   ```bash
   ./update_submodules.sh
   ```

3. **Build the project**:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

## Usage

Here is a simple example of how to create a scene and add a particle with a spring force:

```cpp
#include "vectra/rendering/renderer.h"
#include "vectra/physics/forces/anchored_spring.h"
#include "linkit/linkit.h"

int main()
{
    Renderer scene_renderer(2560, 1440);
    Scene scene;

    GameObject particle;
    particle.rb.transform.position = linkit::Vector3(5, 0, 0);
    scene.add_game_object(particle);

    linkit::Vector3 anchor = linkit::Vector3(0.0, 5.0, 0.0);
    scene.force_registry.add(&scene.game_objects[0], std::make_shared<AnchoredSpring>(anchor, 10.0, 5.0, 1.0));

    scene_renderer.play_scene(scene);
    return 0;
}
```

## Dependencies

- **GLAD**: OpenGL Loading Library.
- **GLFW**: A multi-platform library for OpenGL, OpenGL ES and Vulkan development.
- **GLM**: OpenGL Mathematics.
- **Assimp**: A portable Open-Source library to import various well-known 3D model formats.
- **Linkit**: A custom 3D math library created for this project.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.