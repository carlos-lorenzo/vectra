# Physics Module

The physics module implements rigid body dynamics, force generators, collision detection, and collision response.

## Components

### Rigidbody (`rigidbody.h`, `rigidbody.cpp`)

Represents the physical properties and state of an object.

**Members:**
| Member | Type | Description |
|--------|------|-------------|
| `transform` | `Transform` | Position, rotation, scale |
| `mass` | `real` | Mass in kg (`0` = infinite/static) |
| `inverse_mass` | `real` | Cached `1/mass` for efficiency |
| `velocity` | `Vector3` | Linear velocity (m/s) |
| `angular_velocity` | `Vector3` | Angular velocity (rad/s) |
| `acceleration` | `Vector3` | Linear acceleration (m/s²) |
| `angular_acceleration` | `Vector3` | Angular acceleration (rad/s²) |

**Key Methods:**
```cpp
void add_force(const Vector3& force);                    // Apply force at center of mass
void add_force_at_point(const Vector3& force,            // Apply force at world point
                        const Vector3& world_point);
void add_force_at_local_point(const Vector3& force,      // Apply force at local point
                              const Vector3& local_point);
void integrate(real dt);                                 // Update position/velocity
Matrix3 cuboid_inertia_tensor() const;                   // Compute box inertia
Matrix3 sphere_inertia_tensor() const;                   // Compute sphere inertia
```

**Static Objects:**
Set `mass = 0` to create immovable objects. The `inverse_mass` will be `0`, preventing any motion.

### Transform (`transform.h`, `transform.cpp`)

Represents position, orientation, and scale in 3D space.

**Members:**
| Member | Type | Description |
|--------|------|-------------|
| `position` | `Vector3` | World position |
| `rotation` | `Quaternion` | Orientation |
| `scale` | `Vector3` | Scale factor |

---

## Force Generators

Force generators apply forces to objects each physics step. All inherit from `ForceGenerator`.

### Base Class

```cpp
class ForceGenerator {
public:
    virtual void update_force(GameObject& obj, linkit::real dt) = 0;
};
```

### Built-in Forces

#### SimpleGravity (`forces/simple_gravity.h`)

Applies a constant gravitational field.

```cpp
SimpleGravity(const Vector3& field = Vector3(0, -9.81, 0));
```

**JSON:**
```json
{ "type": "simple_gravity", "field": [0, -9.81, 0] }
```

#### NewtonianGravity (`forces/newtonian_gravity.h`)

Applies N-body gravitational attraction using inverse-square law.

```cpp
NewtonianGravity(std::vector<GameObject>& affected_objects, 
                 real gravitational_constant = 6.67e-11);
```

**JSON:**
```json
{
    "type": "newtonian_gravity",
    "gravitational_constant": 1000.0,
    "affected_object_indices": [0, 1, 2]
}
```

#### AnchoredSpring (`forces/anchored_spring.h`)

Spring force attached to a fixed world point.

```cpp
AnchoredSpring(const Vector3& anchor_point, 
               real spring_constant, 
               real rest_length, 
               real damping);
```

**JSON:**
```json
{
    "type": "anchored_spring",
    "anchor_point": [0, 10, 0],
    "spring_constant": 50.0,
    "rest_length": 5.0,
    "damping": 0.5
}
```

#### ObjectAnchoredSpring (`forces/object_anchored_spring.h`)

Spring force connecting two game objects.

```cpp
ObjectAnchoredSpring(GameObject* anchor_object, 
                     real spring_constant, 
                     real rest_length, 
                     real damping);
```

**JSON:**
```json
{
    "type": "object_anchored_spring",
    "object_index": 0,
    "spring_constant": 100.0,
    "rest_length": 3.0,
    "damping": 1.0
}
```

### Creating Custom Forces

1. Inherit from `ForceGenerator`
2. Implement `update_force()`
3. Register in `SceneSerializer` for JSON support

```cpp
class WindForce : public ForceGenerator {
    Vector3 wind_velocity;
    real drag_coefficient;

public:
    WindForce(Vector3 velocity, real drag)
        : wind_velocity(velocity), drag_coefficient(drag) {}

    void update_force(GameObject& obj, real dt) override {
        Vector3 relative_velocity = wind_velocity - obj.rb.velocity;
        Vector3 force = relative_velocity * drag_coefficient;
        obj.rb.add_force(force);
    }
};
```

### ForceRegistry (`force_registry.h`, `force_registry.cpp`)

Manages object-force bindings and applies forces each step.

```cpp
void add(GameObject* obj, std::shared_ptr<ForceGenerator> force);
void remove(GameObject* obj, std::shared_ptr<ForceGenerator> force);
void update_forces(real dt);  // Called each physics step
```

---

## Collision System

### Broad Phase: BVH (`BVHNode.h`)

Bounding Volume Hierarchy using bounding spheres for efficient culling.

```cpp
BVHNode<BoundingSphere>* insert(GameObject* object, const BoundingSphere& volume);
void get_potential_contacts(std::vector<PotentialContact>& contacts);
```

### Bounding Volumes (`bounding_volumes/`)

| Volume | Description |
|--------|-------------|
| `BoundingSphere` | Sphere bounding volume |

### Colliders (`colliders/`)

| Collider | Description |
|----------|-------------|
| `ColliderBox` | Oriented bounding box |
| `ColliderSphere` | Sphere collider |

### Narrow Phase (`collider_primitive.h`)

Precise collision detection between collider pairs:
- Sphere vs Sphere
- Sphere vs Box
- Box vs Box

### Collision Response

#### CollisionContact (`collision_contact.h`)

Represents a single contact point:
| Member | Type | Description |
|--------|------|-------------|
| `contact_point` | `Vector3` | World position of contact |
| `contact_normal` | `Vector3` | Normal pointing from B to A |
| `penetration` | `real` | Penetration depth |
| `restitution` | `real` | Bounciness (0-1) |
| `friction` | `real` | Friction coefficient |

#### CollisionData (`collision_data.h`)

Container for multiple contacts from a collision pair.

#### CollisionHandler (`collision_handler.h`, `collision_handler.cpp`)

Resolves collisions using impulse-based response:

```cpp
void set_parameters(int pos_iterations, int vel_iterations,
                    real pos_epsilon, real vel_epsilon);
void resolve_contacts(std::vector<CollisionContact>& contacts, real dt);
```

**Resolution Process:**
1. Calculate separating velocity
2. Apply velocity impulse (with restitution)
3. Apply position correction (penetration resolution)
4. Handle friction

---

## Physics Pipeline

Each `Scene::step(dt)` call:

```
1. ForceRegistry::update_forces(dt)
   └── Each ForceGenerator::update_force()

2. Rigidbody::integrate(dt)
   └── Update velocities and positions

3. BVH::get_potential_contacts()
   └── Broad-phase collision detection

4. Narrow-phase collision detection
   └── Generate CollisionContacts

5. CollisionHandler::resolve_contacts()
   └── Apply impulses and corrections
```

---

## Usage Examples

### Applying Forces

```cpp
// Direct force application
obj.rb.add_force(Vector3(100, 0, 0));  // Push right

// Force at point (generates torque)
obj.rb.add_force_at_point(
    Vector3(0, 100, 0),           // Force direction
    obj.rb.transform.position + Vector3(1, 0, 0)  // Application point
);

// Using force generators
scene->force_registry.add(&obj, std::make_shared<SimpleGravity>());
```

### Configuring Collision Response

```cpp
// In Engine constructor
scene->collision_handler.set_parameters(
    10,     // Position iterations
    10,     // Velocity iterations
    0.01,   // Position epsilon
    0.01    // Velocity epsilon
);
```

## Related Documentation

- [Scene Schema](../../docs/SCENE_SCHEMA.md) - Force generator JSON format
- [Core Module](../core/README.md) - Scene and GameObject
- [Rendering Module](../rendering/README.md) - Visual representation
