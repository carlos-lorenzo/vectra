# Rendering Module

The rendering module handles OpenGL-based 3D rendering, shaders, model loading, camera control, and the editor UI.

## Components

### Renderer (`renderer.h`, `renderer.cpp`)

Main rendering controller that manages the OpenGL pipeline.

**Key Methods:**
```cpp
void setup_from_scene(const Scene& scene);              // Load scene assets
void render_to_framebuffer(const SceneSnapshot& snap,   // Render scene
                           real dt);
void use_skybox();                                      // Enable skybox
GLuint get_scene_texture_id() const;                    // Get rendered texture
static void begin_frame();                              // Clear buffers
void end_frame();                                       // Swap buffers
void cleanup(Scene& scene);                             // Release resources
```

**Rendering Pipeline:**
1. Clear framebuffer
2. Set view/projection matrices from camera
3. Render skybox (if enabled)
4. Render each object with appropriate shader
5. Apply lighting

### Framebuffer (`framebuffer.h`, `framebuffer.cpp`)

Off-screen render target for scene rendering (used by ImGui viewport).

```cpp
void create(int width, int height);
void bind();
void unbind();
GLuint get_texture_id() const;
void resize(int width, int height);
```

---

## Shaders (`shader.h`, `shader.cpp`)

GLSL shader program wrapper.

**Key Methods:**
```cpp
void use() const;                                    // Bind shader
void set_bool(const std::string& name, bool value);
void set_int(const std::string& name, int value);
void set_float(const std::string& name, float value);
void set_vec3(const std::string& name, const glm::vec3& value);
void set_mat4(const std::string& name, const glm::mat4& value);
```

**Available Shaders** (in `resources/shaders/`):

| Shader | Description |
|--------|-------------|
| `phong` | Phong lighting with specular highlights |
| `diffuse` | Diffuse-only lighting |
| `debug` | Solid color for debugging |
| `identity` | No transformation (UI elements) |
| `skybox` | Cubemap skybox rendering |

---

## Models (`model.h`, `model.cpp`)

3D model loading and management via Assimp.

**Supported Formats:**
- OBJ, FBX, GLTF/GLB, DAE (Collada), 3DS, and more

**Key Methods:**
```cpp
void load(const std::string& path);
void draw(const Shader& shader) const;
```

### Mesh (`mesh.h`, `mesh.cpp`)

Individual mesh within a model, containing vertices, indices, and textures.

**Vertex Format:**
```cpp
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
};
```

---

## Camera (`camera.h`, `camera.cpp`)

Scene camera with perspective projection.

**Members:**
| Member | Type | Description |
|--------|------|-------------|
| `transform` | `Transform` | Position and orientation |
| `fov` | `float` | Field of view (degrees) |

**Key Methods:**
```cpp
glm::mat4 get_view_matrix() const;
glm::mat4 get_projection_matrix(float aspect_ratio) const;
```

**Default Values:**
- Position: `[0, 0, 0]`
- Rotation: Identity quaternion
- FOV: `90.0`

---

## Lighting (SceneLights)

Scene lighting uses the grouped `SceneLights` API (see `include/vectra/rendering/scene_lights.h` and `include/vectra/rendering/light_sources.h`). Scenes provide a `lights` object with optional `directional_lights`, `point_lights`, and `spot_lights` arrays.

Example: programmatically add lights to a scene

```cpp
DirectionalLight sun(glm::vec3(-0.577f, -0.577f, -0.577f), glm::vec3(1.0f, 1.0f, 0.8f));
scene->add_directional_light(sun);

PointLight p(glm::vec3(0,20,20), glm::vec3(1.0f));
scene->add_point_light(p);

SpotLight spot(glm::vec3(0, 10, 0), glm::vec3(0, -1, 0), glm::vec3(1.0f));
scene->add_spot_light(spot);
```

When loading from JSON, the `lights` object may look like:

```json
"lights": {
  "directional_lights": [ { "direction": [ -0.2, -1.0, -0.3 ], "colour": [1,1,1] } ],
  "point_lights": [ { "position": [0,20,20], "colour": [1,1,1], "distance": 50 } ],
  "spot_lights": [ { "position": [0,10,0], "direction": [0,-1,0], "colour": [1,1,1] } ]
}
```

---

## Skybox (`skybox.h`, `skybox.cpp`)

Cubemap-based environment background.

**Key Methods:**
```cpp
void load(const std::vector<std::string>& faces);  // Load 6 cubemap faces
void draw(const glm::mat4& view, const glm::mat4& projection);
```

**Face Order:**
1. Right (+X)
2. Left (-X)
3. Top (+Y)
4. Bottom (-Y)
5. Front (+Z)
6. Back (-Z)

---

## Engine UI (`engine_ui.h`, `engine_ui.cpp`)

ImGui-based editor interface.

**Panels:**
| Panel | Description |
|-------|-------------|
| **Toolbar** | Play/Pause, speed control, FPS display |
| **Hierarchy** | Object list with selection |
| **Inspector** | Selected object properties |
| **Scene View** | 3D viewport (rendered framebuffer) |

**Key Methods:**
```cpp
static void initialize(GLFWwindow* window);
void draw(EngineState& state, const SceneSnapshot& snapshot, GLuint texture_id);
void end_frame();
void cleanup();
```

**Theme:**
Custom Dracula-inspired dark theme for professional appearance.

---

## Debug Rendering (`debug_drawer.h`, `debug_drawer.cpp`)

Utilities for debug visualization.

**Capabilities:**
- Wireframe rendering
- Collision shape visualization
- Contact point display
- BVH node visualization

---

## Rendering Pipeline

```
┌─────────────────────────────────────────────────────────┐
│                    Render Frame                         │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  1. Renderer::begin_frame()                             │
│     └── glClear(COLOR | DEPTH)                          │
│                                                         │
│  2. Framebuffer::bind()                                 │
│     └── Render to off-screen texture                    │
│                                                         │
│  3. Camera matrices                                     │
│     ├── view = camera.get_view_matrix()                 │
│     └── proj = camera.get_projection_matrix()           │
│                                                         │
│  4. Skybox::draw() (if enabled)                         │
│                                                         │
│  5. For each object:                                    │
│     ├── Shader::use()                                   │
│     ├── Set uniforms (MVP, lighting)                    │
│     └── Model::draw()                                   │
│                                                         │
│  6. Framebuffer::unbind()                               │
│                                                         │
│  7. EngineUI::draw()                                    │
│     └── ImGui renders with framebuffer texture          │
│                                                         │
│  8. Renderer::end_frame()                               │
│     └── glfwSwapBuffers()                               │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

---

## Resource Locations

| Resource | Path |
|----------|------|
| Shaders | `resources/shaders/` |
| Models | `resources/models/` |
| Textures | `resources/textures/` |
| Fonts | `resources/fonts/` |
| Skybox | `resources/textures/skybox/` |

---

## Usage Examples

### Setting Up Lighting

```cpp
// In scene setup
LightSource sun(
    glm::vec3(0, 100, 50),    // Position
    glm::vec3(1.0, 0.95, 0.8) // Warm white
);
scene->add_light_source(sun);

// Add fill light
LightSource fill(
    glm::vec3(-50, 20, -30),
    glm::vec3(0.3, 0.3, 0.4)  // Cool blue tint
);
scene->add_light_source(fill);
```

### Camera Configuration

```cpp
scene->camera.fov = 60.0f;
scene->camera.transform.position = linkit::Vector3(0, 10, 20);
// Look at origin (set rotation quaternion accordingly)
```

### Loading Custom Models

Models in `resources/models/` are automatically available:

```cpp
GameObject obj;
obj.set_shape("my_model");  // Loads resources/models/my_model.obj
```

## Related Documentation

- [Core Module](../core/README.md) - Engine and Scene
- [Physics Module](../physics/README.md) - Simulation system
- [Scene Schema](../../docs/SCENE_SCHEMA.md) - JSON format for cameras/lights
