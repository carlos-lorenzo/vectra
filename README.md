<div align="center">

# ⚡ Vectra

**A bespoke rigid-body physics engine built from scratch in modern C++**

[![C++17](https://img.shields.io/badge/C++-17-blue.svg?style=flat&logo=cplusplus)](https://isocpp.org/)
[![OpenGL](https://img.shields.io/badge/OpenGL-4.1-green.svg?style=flat&logo=opengl)](https://www.opengl.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](./LICENSE)
[![CMake](https://img.shields.io/badge/CMake-4.0+-064F8C.svg?style=flat&logo=cmake)](https://cmake.org/)

*Designed to be simple, extensible, and intuitive for simulations and games.*

🚧 **Work in Progress** — actively developed with new features landing regularly!

---

</div>

## ✨ Features

### 🎯 Physics Simulation

| Feature | Description |
|---------|-------------|
| **Rigid Body Dynamics** | Full 3D rigid body simulation with linear & angular motion |
| **Mass & Inertia** | Realistic mass properties with computed inertia tensors for spheres and boxes |
| **Force Accumulation** | Apply forces at arbitrary world/local points with automatic torque calculation |
| **Velocity Integration** | Semi-implicit Euler integration for position and rotation |

### 💥 Collision Detection & Response

| Feature | Description |
|---------|-------------|
| **Broad Phase** | Bounding Volume Hierarchy (BVH) with bounding spheres for efficient culling |
| **Narrow Phase** | Precise collision detection between spheres and oriented bounding boxes |
| **Contact Resolution** | Impulse-based collision response with restitution and friction |
| **Multi-Contact Handling** | Supports multiple contact points per collision pair |

### 🔧 Extensible Force System

Vectra's force generator architecture is **extremely extensible**, allowing you to simulate virtually any physical phenomenon. Simply inherit from `ForceGenerator` and implement your custom force logic:

```cpp
class ForceGenerator {
public:
    virtual void update_force(GameObject& obj, linkit::real dt) = 0;
};
```

**Built-in Force Generators:**

| Force | Description |
|-------|-------------|
| 🌍 **Simple Gravity** | Uniform gravitational field (e.g., Earth's surface) |
| 🪐 **Newtonian Gravity** | N-body gravitational attraction (inverse square law) |
| 🔗 **Anchored Spring** | Spring force tethered to a fixed anchor point with damping |

> 💡 **Create Your Own:** Wind, drag, buoyancy, magnetism, explosions — the architecture supports any force you can imagine!

### 🎨 Rendering Engine

| Feature | Description |
|---------|-------------|
| **Modern OpenGL** | Core profile 3.3+ rendering pipeline |
| **Shader System** | Modular GLSL shaders (Phong, diffuse, debug, identity) |
| **Model Loading** | Import 3D models via Assimp (OBJ, FBX, GLTF, etc.) |
| **Skybox** | Cubemap-based skybox for immersive environments |
| **Lighting** | Dynamic point light sources with Phong shading |

### 🏗️ Architecture

| Feature | Description |
|---------|-------------|
| **Component-Based Design** | GameObjects composed of Rigidbody, Model, Shader, and Collider |
| **Scene Management** | Organize objects, lights, camera, and forces in a Scene |
| **Force Registry** | Centralized system to bind forces to specific objects |
| **Multithreaded Engine** | Decoupled physics and rendering on separate threads with thread-safe render queue |
| **ImGui Integration** | Real-time debug UI for engine state, simulation controls, and inspection |

---

## 🚀 Getting Started

### Prerequisites

| Requirement | Version |
|-------------|---------|
| C++ Compiler | C++17 support |
| CMake | 4.0+ |
| OpenGL | 3.3+ |

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/your-username/vectra.git
cd vectra

# Initialize submodules
./update_submodules.sh

# Build
mkdir build && cd build
cmake ..
make
```

---

## 📖 Usage

### Basic Example

The engine handles scene setup, physics stepping, and rendering automatically:

```cpp
#include "vectra/core/engine.h"

int main()
{
    Engine engine;
    engine.load_scene();  // Load your scene configuration
    engine.run();         // Multithreaded: physics + rendering
    return 0;
}
```

### Scene Configuration

Configure scenes by adding GameObjects with physics properties and forces:

```cpp
void Engine::load_scene()
{
    // Add lighting
    LightSource light(glm::vec3(0, 10, -10), glm::vec3(1.0, 1.0, 0.8));
    scene->add_light_source(light);

    // Create a static floor
    GameObject floor;
    floor.rb.mass = 0;  // Infinite mass = static object
    floor.rb.inverse_mass = 0;
    floor.rb.transform.position = linkit::Vector3(0, -10, 0);
    floor.rb.transform.scale = linkit::Vector3(20, 1, 20);
    floor.set_collider_type("ColliderBox");
    floor.set_shape("cube");
    scene->add_game_object(floor);

    // Create dynamic spheres with gravity
    linkit::Vector3 gravity(0, -9.8, 0);
    for (int i = 0; i < 10; i++)
    {
        GameObject sphere;
        sphere.set_collider_type("ColliderSphere");
        sphere.set_shape("sphere");
        sphere.rb.transform.position = linkit::Vector3(0, 5 + i * 2, 0);
        scene->add_game_object(sphere);

        // Register gravity force for this object
        scene->force_registry.add(
            &scene->game_objects.back(),
            std::make_shared<SimpleGravity>(gravity)
        );
    }
}
```

### Creating Custom Forces

Extend the physics system with your own forces:

```cpp
#include "vectra/physics/force_generator.h"

class WindForce : public ForceGenerator {
    linkit::Vector3 wind_velocity;
    linkit::real drag_coefficient;

public:
    WindForce(linkit::Vector3 velocity, linkit::real drag)
        : wind_velocity(velocity), drag_coefficient(drag) {}

    void update_force(GameObject& obj, linkit::real dt) override {
        linkit::Vector3 relative_velocity = wind_velocity - obj.rb.velocity;
        linkit::Vector3 force = relative_velocity * drag_coefficient;
        obj.rb.add_force(force);
    }
};
```

---

## 📦 Dependencies

| Library | Purpose |
|---------|---------|
| [GLAD](https://glad.dav1d.de/) | OpenGL function loading |
| [GLFW](https://www.glfw.org/) | Window creation & input handling |
| [GLM](https://github.com/g-truc/glm) | OpenGL mathematics |
| [Assimp](https://github.com/assimp/assimp) | 3D model import |
| [ImGui](https://github.com/ocornut/imgui) | Immediate mode GUI |
| [Linkit](./external/linkit) | Custom 3D math library (vectors, matrices, quaternions) |

---

## 🗂️ Project Structure

```
vectra/
├── include/vectra/          # Public headers
│   ├── core/                # Engine, Scene, GameObject
│   ├── physics/             # Rigidbody, Forces, Colliders, BVH
│   └── rendering/           # Renderer, Shaders, Camera, Skybox
├── src/                     # Implementation files
├── resources/
│   ├── models/              # 3D model assets
│   ├── shaders/             # GLSL shader programs
│   └── textures/            # Textures and skybox cubemaps
└── external/                # Third-party dependencies
```

---

## 🚧 Work in Progress

Vectra is under active development. Here's what's on the roadmap:

| Feature | Status |
|---------|--------|
| **Constraint System** | Joint constraints, hinges, and distance limits |
| **Friction Collisions** | Enhanced contact solver with static/dynamic friction |
| **Resting Contacts** | Improved stability for stacked and resting objects |
| **Scene API Refactor** | Cleaner scene loading and configuration interface |
| **UI Overhaul** | Expanded ImGui panels for debugging and visualization |
| **Model Optimization** | Faster asset loading and GPU mesh management |

> 🛠️ Contributions and feedback are welcome!

---

## 📄 License

This project is licensed under the **MIT License** — see the [LICENSE](./LICENSE) file for details.

---

<div align="center">

**Built with ❤️ and physics**

</div>
