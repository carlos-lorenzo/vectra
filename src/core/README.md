# Core Module

The core module contains the central components that tie together the physics and rendering systems.

## Components

### Engine (`engine.h`, `engine.cpp`)

The main application controller that orchestrates physics simulation and rendering.

**Key Features:**
- Multithreaded architecture with separate physics and rendering threads
- Scene loading and saving via `SceneSerializer`
- Configurable simulation parameters (frequency, speed, pause)
- Thread-safe communication via `RenderQueue`

**Key Methods:**
```cpp
void load_scene(const std::string& filename);  // Load scene from JSON file
void run();                                     // Start multithreaded engine
void run_single_thread();                       // Single-threaded mode (debugging)
```

**Threading Model:**
- **Physics Thread**: Runs at fixed frequency (default 60Hz), updates forces and resolves collisions
- **Rendering Thread**: Runs on main thread (GLFW requirement), renders scene snapshots

### Scene (`scene.h`, `scene.cpp`)

Container for all simulation objects and systems.

**Members:**
| Member | Type | Description |
|--------|------|-------------|
| `name` | `string` | Scene name |
| `game_objects` | `deque<GameObject>` | All objects (deque for pointer stability) |
| `light_sources` | `vector<LightSource>` | Scene lights |
| `camera` | `Camera` | View camera |
| `skybox` | `Skybox` | Environment skybox |
| `force_registry` | `ForceRegistry` | Object-force bindings |
| `bvh_root` | `unique_ptr<BVHNode>` | Collision broad-phase tree |
| `collision_handler` | `CollisionHandler` | Collision resolution system |

**Key Methods:**
```cpp
void add_game_object(GameObject obj);      // Add object with auto-naming
void add_light_source(const LightSource&); // Add light
void step(linkit::real dt);                // Advance simulation
SceneSnapshot create_snapshot() const;     // Thread-safe state copy
```

**Auto-Naming:**
Objects without names are automatically named using `{model_name}_{index}` (e.g., `sphere_0`, `cube_1`).

### GameObject (`gameobject.h`, `gameobject.cpp`)

Represents a physical object in the scene.

**Members:**
| Member | Type | Description |
|--------|------|-------------|
| `name` | `string` | Object identifier |
| `model_name` | `string` | Mesh type (`"cube"`, `"sphere"`) |
| `rb` | `Rigidbody` | Physics properties |
| `collider` | `unique_ptr<Collider>` | Collision shape |

**Key Methods:**
```cpp
void set_collider_type(const std::string& type);  // "ColliderBox" or "ColliderSphere"
void set_shape(const std::string& shape);         // Set model mesh
Collider& get_collider();                         // Access collider
```

### SceneSerializer (`scene_serializer.h`, `scene_serializer.cpp`)

Handles JSON serialization/deserialization of scenes.

**Key Methods:**
```cpp
void serialize_scene(const Scene& scene, const std::string& filename);
SceneLoadResult deserialize_scene(const std::string& filename);
```

**SceneLoadResult:**
```cpp
struct SceneLoadResult {
    Scene scene;
    std::vector<std::string> warnings;  // Non-fatal issues
    std::vector<std::string> errors;    // Fatal issues
    
    bool has_errors() const;
    bool has_warnings() const;
};
```

**Default Values:**
All fields have sensible defaults, allowing minimal JSON files. See [docs/SCENE_SCHEMA.md](../../docs/SCENE_SCHEMA.md) for complete schema.

**Warning/Error Handling:**
- Warnings: Unknown force types, invalid object indices (logged, scene still loads)
- Errors: JSON parse errors, file not found (scene loading aborted)

## Architecture Diagram

```
┌─────────────────────────────────────────────────────────┐
│                        Engine                           │
│  ┌─────────────────┐           ┌─────────────────────┐  │
│  │  Physics Thread │           │  Rendering Thread   │  │
│  │                 │           │                     │  │
│  │  Scene::step()  │──────────▶│  Renderer::render() │  │
│  │                 │  Snapshot │                     │  │
│  └─────────────────┘           └─────────────────────┘  │
│           │                             │               │
│           ▼                             ▼               │
│  ┌─────────────────┐           ┌─────────────────────┐  │
│  │  ForceRegistry  │           │      EngineUI       │  │
│  │  CollisionHdlr  │           │                     │  │
│  └─────────────────┘           └─────────────────────┘  │
└─────────────────────────────────────────────────────────┘
```

## Usage Examples

### Loading a Scene from JSON

```cpp
Engine engine;
engine.load_scene("my_scene.json");  // Loads from resources/scenes/
engine.run();
```

### Programmatic Scene Setup

```cpp
// Create static ground
GameObject ground;
ground.rb.mass = 0;  // Static object
ground.rb.transform.scale = linkit::Vector3(100, 1, 100);
ground.set_collider_type("ColliderBox");
ground.set_shape("cube");
scene->add_game_object(ground);

// Create dynamic sphere with gravity
GameObject sphere;
sphere.rb.mass = 1.0;
sphere.rb.transform.position = linkit::Vector3(0, 10, 0);
sphere.set_collider_type("ColliderSphere");
sphere.set_shape("sphere");
scene->add_game_object(sphere);

// Add gravity force
scene->force_registry.add(
    &scene->game_objects.back(),
    std::make_shared<SimpleGravity>(linkit::Vector3(0, -9.81, 0))
);
```

## Related Documentation

- [Scene Schema](../../docs/SCENE_SCHEMA.md) - JSON format specification
- [Physics Module](../physics/README.md) - Rigidbody and forces
- [Rendering Module](../rendering/README.md) - Rendering pipeline
