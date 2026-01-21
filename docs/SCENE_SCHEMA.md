# Vectra Scene Schema Documentation

This document describes the JSON schema for Vectra scene files. This format is designed for both programmatic generation (including LLMs) and manual editing.

## Overview

Scene files use JSON format and describe a complete physics simulation scene including:
- Scene metadata
- Camera configuration
- Game objects with physics properties
- Force generators applied to objects
- Scene lights (directional, point, spot)

## Schema Reference

### Scene (Root Object)

| Field | Type | Required | Default | Description |
|-------|------|----------|---------|-------------|
| `name` | string | No | `"New Scene"` | Human-readable scene name |
| `camera` | Camera | No | Default camera | Scene camera configuration |
| `objects` | array[GameObject] | No | `[]` | Game objects in the scene |
| `lights` | SceneLights | No | `{}` | Grouped lights object containing directional/point/spot light arrays |

### Camera

| Field | Type | Required | Default | Description |
|-------|------|----------|---------|-------------|
| `fov` | number | No | `90.0` | Field of view in degrees |
| `transform` | Transform | No | Default transform | Camera position and orientation |

### Transform

| Field | Type | Required | Default | Description |
|-------|------|----------|---------|-------------|
| `position` | Vector3 | No | `[0, 0, 0]` | World position (x, y, z) |
| `rotation` | Quaternion or Euler | No | `[1, 0, 0, 0]` | Rotation as quaternion (w, x, y, z) or Euler angles (pitch, yaw, roll) in radians |
| `scale` | Vector3 | No | `[1, 1, 1]` | Scale factor (x, y, z) |

### GameObject

| Field | Type | Required | Default | Description |
|-------|------|----------|---------|-------------|
| `name` | string | No | Auto-generated | Object name. If omitted, generated as `{model_name}_{index}` (e.g., `"sphere_0"`, `"cube_1"`) |
| `model_name` | string | No | `"cube"` | Mesh model to use. Options: `"cube"`, `"sphere"` |
| `collider_type` | string | No | `"ColliderBox"` | Collision shape. Options: `"ColliderBox"`, `"ColliderSphere"` |
| `rigidbody` | Rigidbody | No | Default rigidbody | Physics properties |
| `force_generators` | array[ForceGenerator] | No | `[]` | Forces applied to this object |

### Rigidbody

| Field | Type | Required | Default | Description |
|-------|------|----------|---------|-------------|
| `transform` | Transform | No | Default transform | Object transform (position, rotation, scale) |
| `mass` | number | No | `0` | Mass in kg. `0` = infinite mass (static object) |
| `velocity` | Vector3 | No | `[0, 0, 0]` | Initial linear velocity (m/s) |
| `angular_velocity` | Vector3 | No | `[0, 0, 0]` | Initial angular velocity (rad/s) |
| `acceleration` | Vector3 | No | `[0, 0, 0]` | Initial linear acceleration (m/s²) |
| `angular_acceleration` | Vector3 | No | `[0, 0, 0]` | Initial angular acceleration (rad/s²) |

### SceneLights

The `lights` field is a single object that groups three optional arrays:
- `directional_lights`: array[DirectionalLight]
- `point_lights`: array[PointLight]
- `spot_lights`: array[SpotLight]

If the `lights` object is omitted the scene uses engine defaults (no explicit lights).

#### DirectionalLight

| Field | Type | Required | Default | Description |
|-------|------|----------|---------|-------------|
| `direction` | Vector3 | No | `[-0.2, -1.0, -0.3]` | Direction the light is shining |
| `colour` | Vector3 | No | `[1, 1, 1]` | Shorthand colour applied to ambient/diffuse/specular (preferred)
| `ambient` | Vector3 | No | `[1, 1, 1]` | Ambient component |
| `diffuse` | Vector3 | No | `[1, 1, 1]` | Diffuse component |
| `specular` | Vector3 | No | `[1, 1, 1]` | Specular component |
| `strength` | number | No | `0.5` | Intensity multiplier |

If `colour` is supplied it overrides `ambient`/`diffuse`/`specular` (all set to that colour).

#### PointLight

| Field | Type | Required | Default | Description |
|-------|------|----------|---------|-------------|
| `position` | Vector3 | No | `[0, 0, 0]` | World position |
| `colour` | Vector3 | No | `[1, 1, 1]` | Shorthand colour; ambient = colour * 0.1, diffuse/specular = colour |
| `ambient` | Vector3 | No | `[0.05, 0.05, 0.05]` | Ambient component |
| `diffuse` | Vector3 | No | `[0.8, 0.8, 0.8]` | Diffuse component |
| `specular` | Vector3 | No | `[1, 1, 1]` | Specular component |
| `distance` | number | No | - | If supplied, `set_attenuation(distance)` is called to derive `constant/linear/quadratic` |
| `constant` | number | No | `1.0` | Attenuation constant term (overrides `distance` if present) |
| `linear` | number | No | `0.09` | Attenuation linear term (overrides `distance` if present) |
| `quadratic` | number | No | `0.032` | Attenuation quadratic term (overrides `distance` if present) |

Note: `distance` is a convenience to pick standard attenuation from a table; explicit `constant`/`linear`/`quadratic` values override it.

#### SpotLight

| Field | Type | Required | Default | Description |
|-------|------|----------|---------|-------------|
| `position` | Vector3 | No | `[0, 0, 0]` | World position |
| `direction` | Vector3 | No | `[0, -1, 0]` | Direction the spotlight is pointing |
| `cut_off` | number | No | `0.9` | Inner cone cosine |
| `outer_cut_off` | number | No | `0.85` | Outer cone cosine |
| `colour` | Vector3 | No | `[1, 1, 1]` | Shorthand colour (ambient scaled) |
| `ambient` | Vector3 | No | `[0, 0, 0]` | Ambient component |
| `diffuse` | Vector3 | No | `[1, 1, 1]` | Diffuse component |
| `specular` | Vector3 | No | `[1, 1, 1]` | Specular component |
| `distance` | number | No | - | If supplied, `set_attenuation(distance)` is called to derive `constant/linear/quadratic` |
| `constant` | number | No | `1.0` | Attenuation constant term (overrides `distance` if present) |
| `linear` | number | No | `0.09` | Attenuation linear term (overrides `distance` if present) |
| `quadratic` | number | No | `0.032` | Attenuation quadratic term (overrides `distance` if present) |

---

## Primitive Types

### Vector3
A 3-element array: `[x, y, z]`

Example: `[0.0, 10.0, -5.0]`

### Quaternion
A 4-element array representing rotation: `[w, x, y, z]`

Identity (no rotation): `[1, 0, 0, 0]`

### Euler Angles (Alternative to Quaternion)
A 3-element array: `[pitch, yaw, roll]` in radians.

Automatically converted to quaternion during loading.

---

## Force Generators

Force generators define physics forces applied to objects. Each has a `type` field that determines its behavior.

### Common Structure

```json
{
    "type": "<force_type>",
    // ... type-specific fields
}
```

### simple_gravity

Applies a constant gravitational field to the object.

| Field | Type | Required | Default | Description |
|-------|------|----------|---------|-------------|
| `type` | string | **Yes** | - | Must be `"simple_gravity"` |
| `field` | Vector3 | No | `[0, 0, 0]` | Gravitational acceleration vector (m/s²). Earth gravity: `[0, -9.81, 0]` |

**Example:**
```json
{
    "type": "simple_gravity",
    "field": [0, -9.81, 0]
}
```

### newtonian_gravity

Applies Newtonian gravitational attraction between this object and other specified objects.

| Field | Type | Required | Default | Description |
|-------|------|----------|---------|-------------|
| `type` | string | **Yes** | - | Must be `"newtonian_gravity"` |
| `gravitational_constant` | number | No | `0` | Gravitational constant G (default: `6.67e-11` for realistic physics) |
| `affected_object_indices` | array[int] | **Yes** | - | Indices of objects that exert gravitational pull on this object |

**Example:**
```json
{
    "type": "newtonian_gravity",
    "gravitational_constant": 6.67e-11,
    "affected_object_indices": [0, 1, 2]
}
```

**Note:** Object indices are 0-based and refer to the order objects appear in the `objects` array.

### anchored_spring

Applies a spring force connecting the object to a fixed point in world space.

| Field | Type | Required | Default | Description |
|-------|------|----------|---------|-------------|
| `type` | string | **Yes** | - | Must be `"anchored_spring"` |
| `anchor_point` | Vector3 | No | `[0, 0, 0]` | Fixed anchor position in world space |
| `spring_constant` | number | No | `0` | Spring stiffness (N/m) |
| `rest_length` | number | No | `0` | Natural length of spring (m) |
| `damping` | number | No | `0` | Damping coefficient |

**Example:**
```json
{
    "type": "anchored_spring",
    "anchor_point": [0, 10, 0],
    "spring_constant": 50.0,
    "rest_length": 5.0,
    "damping": 0.5
}
```

### object_anchored_spring

Applies a spring force connecting the object to another game object.

| Field | Type | Required | Default | Description |
|-------|------|----------|---------|-------------|
| `type` | string | **Yes** | - | Must be `"object_anchored_spring"` |
| `object_index` | int | **Yes** | - | Index of the anchor object |
| `spring_constant` | number | No | `0` | Spring stiffness (N/m) |
| `rest_length` | number | No | `0` | Natural length of spring (m) |
| `damping` | number | No | `0` | Damping coefficient |

**Example:**
```json
{
    "type": "object_anchored_spring",
    "object_index": 0,
    "spring_constant": 100.0,
    "rest_length": 3.0,
    "damping": 1.0
}
```

---

## Object Indexing Convention

Objects are referenced by their **0-based index** in the `objects` array. This is used by:
- `newtonian_gravity.affected_object_indices`
- `object_anchored_spring.object_index`

**Important:** When referencing objects, ensure the referenced object appears **before** the referencing force in the objects array, or at least in the same scene load.

---

## Warnings and Errors

### Warnings (Non-Fatal)

Warnings are logged but do not prevent scene loading:

| Condition | Warning Message |
|-----------|-----------------|
| Unknown force type | `Warning: Unknown force type 'X' on object 'Y', skipping` |
| Force generator missing `type` field | `Warning: Force generator on object 'Y' has no type, skipping` |
| Invalid object index | `Warning: Invalid object index N for newtonian_gravity on object 'Y', ignoring index` |
| `newtonian_gravity` with no valid objects | `Warning: newtonian_gravity on object 'Y' has no valid affected objects, skipping` |
| `object_anchored_spring` missing `object_index` | `Warning: object_anchored_spring on object 'Y' has no object_index, skipping` |
| `object_anchored_spring` with invalid index | `Warning: object_anchored_spring on object 'Y' has invalid anchor index N, skipping` |

### Errors (Fatal)

Errors prevent scene loading:

| Condition | Error Message |
|-----------|---------------|
| JSON parse error | `Error: JSON parse error in file 'X': <details>` |
| JSON type error | `Error: JSON type error in file 'X': <details>` |
| File not found | `Error: Could not open file: <path>` |

---

## Examples

### Minimal Scene (Using Defaults)

```json
{
    "objects": [
        {
            "model_name": "sphere",
            "rigidbody": {
                "transform": {
                    "position": [0, 10, 0]
                },
                "mass": 1.0
            },
            "force_generators": [
                {
                    "type": "simple_gravity",
                    "field": [0, -9.81, 0]
                }
            ]
        }
    ],
    "lights": {
        "point_lights": [
            {
                "position": [0, 20, 20],
                "colour": [1.0, 1.0, 1.0]
            }
        ]
    }
}
```

### Complete Scene (All Fields Explicit)

```json
{
    "name": "Complete Example Scene",
    "camera": {
        "fov": 90.0,
        "transform": {
            "position": [0, 15, 25],
            "rotation": [1, 0, 0, 0],
            "scale": [1, 1, 1]
        }
    },
    "objects": [ /* ... */ ],
    "lights": {
        "point_lights": [
            {
                "position": [0, 20, 20],
                "colour": [1.0, 1.0, 0.9]
            },
            {
                "position": [-20, 15, -10],
                "colour": [0.5, 0.5, 0.6]
            }
        ]
    }
}
```

### Spring System Example

```json
{
    "name": "Spring Pendulum",
    "objects": [ /* ... */ ],
    "lights": {
        "point_lights": [
            {
                "position": [10, 20, 10],
                "colour": [1, 1, 1]
            }
        ]
    }
}
```

---

## Extending with Custom Forces

To add a new force type:

1. **Implement the ForceGenerator subclass** in `src/physics/forces/`
2. **Add serialization functions** in `scene_serializer.cpp`:
   - `void my_force_to_json(json &j, const MyForce &force)`
   - `void my_force_from_json(const json &j, MyForce &force)` or
   - `bool my_force_from_json(const json &j, MyForce &force, const Scene &scene, const std::string& object_name)` if it references other objects
3. **Register the type** in `from_json(const json &j, Scene &scene)` by adding an `else if (type == "my_force")` branch
4. **Document the type** by adding it to this schema document

Unknown force types will emit a warning and be skipped, allowing scenes to remain forward-compatible.
