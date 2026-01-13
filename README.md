<div align="center">

# ⚡ Vectra

### A Bespoke Rigid-Body Physics Engine Built from Scratch in Modern C++

[![C++17](https://img.shields.io/badge/C++-17-blue.svg?style=flat&logo=cplusplus)](https://isocpp.org/)
[![OpenGL](https://img.shields.io/badge/OpenGL-4.1-green.svg?style=flat&logo=opengl)](https://www.opengl.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](./LICENSE)
[![CMake](https://img.shields.io/badge/CMake-4.0+-064F8C.svg?style=flat&logo=cmake)](https://cmake.org/)
[![Status](https://img.shields.io/badge/Status-Work%20in%20Progress-orange.svg)](.)

<br>

*Simple. Extensible. Intuitive.*

**Simulate virtually any physical phenomenon** — from planetary orbits to spring systems —  
with an architecture designed for **maximum flexibility** and ease of extension.

<br>

🚧 **Actively Developed** — new features landing regularly!

---

</div>

## 🌟 Highlights

<table>
<tr>
<td width="50%">

### 🔧 Infinitely Extensible
Add **any force** you can imagine — gravity, wind, magnetism, explosions, drag, buoyancy — with a simple interface.

</td>
<td width="50%">

### ⚡ Multithreaded Engine
Physics and rendering run on **separate threads** for maximum performance and responsiveness.

</td>
</tr>
<tr>
<td width="50%">

### 🎨 Clean Modern UI
**Unity/Godot-inspired** interface with dockable panels, object hierarchy, inspector, and real-time controls.

</td>
<td width="50%">

### 💥 Robust Collision System
BVH broad-phase, precise narrow-phase detection, and impulse-based response with multi-contact handling.

</td>
</tr>
</table>

---

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

<div align="center">

> **The heart of Vectra's flexibility** — simulate virtually any physical scenario  
> by creating custom forces with a single method override.

</div>

Vectra's force generator architecture makes it trivially easy to add new physics behaviors. Whether you need wind resistance, magnetic fields, buoyancy, explosions, or even fictional forces — simply inherit from `ForceGenerator`:

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
| 🔗 **Object Anchored Spring** | Spring force connecting two GameObjects with configurable stiffness |

<details>
<summary><b>💡 Example: Creating a Wind Force</b></summary>

```cpp
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

</details>

> 🚀 **The possibilities are endless:** atmospheric drag, rocket thrust, vortex fields, gravitational lensing, tractor beams — if you can calculate it, Vectra can simulate it!

### 🎨 Rendering Engine

| Feature | Description |
|---------|-------------|
| **Modern OpenGL** | Core profile 3.3+ rendering pipeline |
| **Shader System** | Modular GLSL shaders (Phong, diffuse, debug, identity) |
| **Model Loading** | Import 3D models via Assimp (OBJ, FBX, GLTF, etc.) with optimized loading |
| **Skybox** | Cubemap-based skybox for immersive environments |
| **Lighting** | Dynamic point light sources with Phong shading |

### 🖥️ Modern Editor Interface

Vectra features a **clean, professional UI** inspired by Unity and Godot, built with ImGui and a custom Dracula-based theme:

| Panel | Description |
|-------|-------------|
| **Scene View** | Real-time 3D viewport with camera controls |
| **Hierarchy** | Browse and select all scene objects |
| **Inspector** | View transform, rigidbody, and model properties |
| **Toolbar** | Play/Pause, simulation speed control, FPS display |

The interface is fully **dockable and customizable**, allowing you to arrange panels to suit your workflow.

### 🏗️ Architecture

| Feature | Description |
|---------|-------------|
| **Component-Based Design** | GameObjects composed of Rigidbody, Model, Shader, and Collider |
| **Scene Management** | Organize objects, lights, camera, and forces in a Scene |
| **Force Registry** | Centralized system to bind forces to specific objects |
| **Multithreaded Engine** | Physics and rendering run on separate threads with a lock-free render queue |
| **Thread-Safe Snapshots** | Scene state safely communicated between threads without blocking |
| **ImGui Integration** | Real-time debug UI with dockable panels and custom theming |

---

## 🚀 Getting Started

### Prerequisites

| Requirement | Version |
|-------------|---------|
| C++ Compiler | C++17 support (GCC 7+, Clang 5+, MSVC 2017+) |
| CMake | 3.16+ |
| OpenGL | 3.3+ |
| Git | For cloning with submodules |

### Build Instructions

#### Linux / macOS

```bash
# Clone the repository with submodules
git clone --recursive https://github.com/carlos-lorenzo/vectra.git
cd vectra

# If you already cloned without --recursive, run:
./update_submodules.sh

# Build
mkdir build && cd build
cmake ..
make -j$(nproc)
```

#### Windows

```powershell
# Clone the repository with submodules
git clone --recursive https://github.com/carlos-lorenzo/vectra.git
cd vectra

# If you already cloned without --recursive, run:
update_submodules.bat

# Build with Visual Studio
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

> ⚠️ **Important:** Always clone with `--recursive` or run the appropriate `update_submodules` script before building. This ensures ImGui and Linkit submodules are properly initialized.

---

## 📖 Usage

### Quick Start

Getting started with Vectra is simple:

```cpp
#include "vectra/core/engine.h"

int main()
{
    auto engine = std::make_unique<Engine>();
    engine->load_scene();  // Configure your scene
    engine->run();         // Launches multithreaded physics + rendering
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

    // Create a static floor (mass = 0 means infinite mass / static)
    GameObject floor;
    floor.rb.mass = 0;
    floor.rb.inverse_mass = 0;
    floor.rb.transform.position = linkit::Vector3(0, -10, 0);
    floor.rb.transform.scale = linkit::Vector3(20, 1, 20);
    floor.set_collider_type("ColliderBox");
    floor.set_shape("cube");
    scene->add_game_object(floor);

    // Create dynamic spheres with gravity
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
            std::make_shared<SimpleGravity>(linkit::Vector3(0, -9.8, 0))
        );
    }
}
```

### Creating Custom Forces

See the [Extensible Force System](#-extensible-force-system) section above for examples of creating custom forces like wind, drag, and more.

---

## 📦 Dependencies

All dependencies are automatically handled — no manual installation required!

| Library | Purpose | Source |
|---------|---------|--------|
| [GLAD](https://glad.dav1d.de/) | OpenGL function loading | Included in repo |
| [GLFW](https://www.glfw.org/) | Window creation & input handling | Auto-fetched (FetchContent) |
| [GLM](https://github.com/g-truc/glm) | OpenGL mathematics | Auto-fetched (FetchContent) |
| [Assimp](https://github.com/assimp/assimp) | 3D model import | Auto-fetched (FetchContent) |
| [ImGui](https://github.com/ocornut/imgui) | Immediate mode GUI | Git submodule |
| [Linkit](https://github.com/carlos-lorenzo/linkit) | Custom 3D math library | Git submodule |

> 💡 **Note:** GLFW, GLM, and Assimp are downloaded automatically during the first CMake configuration. ImGui and Linkit require running `update_submodules.sh` (Linux/macOS) or `update_submodules.bat` (Windows) if you didn't clone with `--recursive`.

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

## 🚧 Roadmap

Vectra is under active development. Here's what's currently in progress:

### 🔨 In Development

| Feature | Description |
|---------|-------------|
| 🔗 **Constraint System** | Joint constraints, hinges, distance limits, and rigid connections |
| 🧱 **Resting Contacts** | Improved stability for stacked and resting objects |
| 🎚️ **Friction Collisions** | Enhanced contact solver with static and dynamic friction |
| 📁 **Scene API Refactor** | Cleaner scene loading, saving, and configuration interface |

### ✅ Recently Completed

| Feature | Description |
|---------|-------------|
| 🎨 **UI Overhaul** | Clean, Unity/Godot-inspired interface with dockable panels |
| ⚡ **Model Optimization** | Faster asset loading and efficient GPU mesh management |
| 🧵 **Multithreading** | Decoupled physics and rendering on separate threads |

> 🛠️ **Contributions and feedback are welcome!**

---

## 📄 License

This project is licensed under the **MIT License** — see the [LICENSE](./LICENSE) file for details.

---

<div align="center">

### Built with ❤️ and physics

**⭐ Star this repo if you find it useful!**

<br>

*Vectra — because physics should be as flexible as your imagination.*

</div>
