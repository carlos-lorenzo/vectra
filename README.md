# Vectra

## Introduction
Vectra is a modern, extensible rigid-body physics engine written in C++. Designed for flexibility and ease of use, it enables simulation of a wide range of physical phenomena, from simple object interactions to complex systems. With a clean architecture and multithreaded design, Vectra is ideal for both real-time applications and academic exploration.

---

## Features

### Physics Simulation
- **Rigid Body Dynamics**: Simulate linear and angular motion with realistic mass and inertia properties.
- **Force Accumulation**: Apply forces at arbitrary points with automatic torque calculation.
- **Collision Detection**: Efficient broad-phase culling and precise narrow-phase detection.
- **Impulse-Based Response**: Handle collisions with restitution and friction (friction is being implemented).

### Scene Management
- **JSON-Based Serialization**: Save and load scenes with a human-readable format.
- **Smart Defaults**: Minimal configuration required for quick prototyping.
- **Error Handling**: Comprehensive validation for scene files.

### Rendering
- **Modern OpenGL**: Core profile rendering pipeline with dynamic lighting and skyboxes.
- **Model Loading**: Import 3D assets in various formats (OBJ, FBX, GLTF).
- **Customizable Shaders**: Modular GLSL shaders for diverse visual effects.

### Extensibility
- **Custom Forces**: Easily implement new force generators for unique simulations.
- **Component-Based Design**: Extend objects with modular components.
- **Multithreaded Architecture**: Decoupled physics and rendering for optimal performance.

---

## Getting Started

### Prerequisites
- **C++ Compiler**: C++17 support (e.g., GCC 7+, Clang 5+, MSVC 2017+).
- **CMake**: Version 3.16 or higher.
- **OpenGL**: Version 3.3 or higher.
- **Git**: For cloning the repository.

### Build Instructions

#### Linux / macOS
```bash
# Clone the repository with submodules
git clone --recursive https://github.com/carlos-lorenzo/vectra.git
cd vectra

# Initialize submodules if not cloned recursively
./update_submodules.sh

# Build the project
mkdir build && cd build
cmake ..
make -j$(nproc)
```

#### Windows
```powershell
# Clone the repository with submodules
git clone --recursive https://github.com/carlos-lorenzo/vectra.git
cd vectra

# Initialize submodules if not cloned recursively
update_submodules.bat

# Build with Visual Studio
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

---

## Usage

### Quick Start
Load and run a default scene:
```cpp
#include "vectra/core/engine.h"

int main() {
    auto engine = std::make_unique<Engine>();
    engine->load_scene("default_scene.json");
    engine->run();
    return 0;
}
```

### Scene Example
```json
{
    "name": "Sample Scene",
    "objects": [
        {
            "name": "ground",
            "rigidbody": {
                "mass": 0,
                "transform": { "scale": [100, 1, 100] }
            }
        },
        {
            "name": "sphere",
            "rigidbody": {
                "mass": 1.0,
                "transform": { "position": [0, 10, 0] }
            }
        }
    ]
}
```

---

## Documentation
- **[Scene Schema](./docs/SCENE_SCHEMA.md)**: Detailed JSON format specification.
- **[Core Module](./src/core/README.md)**: Engine and scene management.
- **[Physics Module](./src/physics/README.md)**: Rigidbody dynamics and collision handling.
- **[Rendering Module](./src/rendering/README.md)**: Graphics pipeline and shaders.

---

## Roadmap

### In Development
- **Shadow Mapping**: Real-time shadows for enhanced realism.
- **Constraint System**: Joints, hinges, and rigid connections.
- **Friction Model**: Improved static and dynamic friction handling.

### Completed
- **Scene Serialization**: JSON-based save/load with error handling.
- **UI Overhaul**: Dockable panels inspired by Unity/Godot.
- **Multithreading**: Decoupled physics and rendering.

---

## License
This project is licensed under the MIT License. See the [LICENSE](./LICENSE) file for details.

---

## Screenshots

### Editor Interface
![Editor Interface](docs/screenshots/editor_interface.png)

*The Unity/Godot-inspired editor with dockable panels.*

### Forces Visualization
![Forces Visualization](docs/screenshots/editor_interface_with_forces.png)

*Real-time visualization of forces acting on objects.*


### Inspector Panel
![Inspector Panel](docs/screenshots/inspector.png)

*View object properties such as transforms, rigidbodies, and model.*

### Scene Loading
![Scene Loading](docs/screenshots/scene_loading.png)

*Load and manage scenes with a clean and intuitive interface.*

### Toolbar
![Toolbar](docs/screenshots/toolbar.png)

*Control simulation playback, speed, and view performance metrics.*

---

## Impulse-Based Physics
Vectra employs an impulse-based approach for collision response, ensuring realistic interactions between objects. This method calculates the change in velocity required to resolve collisions and applies impulses directly to the objects involved. Key aspects include:

- **Contact Resolution**: Resolves collisions by applying impulses to separate objects and prevent interpenetration.
- **Restitution**: Simulates bounciness by adjusting the impulse based on the coefficient of restitution.
- **Friction Handling**: Incorporates frictional forces to simulate realistic sliding and rolling behaviors.
- **Multi-Contact Support**: Handles multiple contact points for complex interactions, such as stacked objects.

This approach is computationally efficient and well-suited for real-time simulations, making it ideal for games and interactive applications.
