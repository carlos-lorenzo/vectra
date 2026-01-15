#include "vectra/core/scene_serializer.h"

#include <iostream>
#include <fstream>
#include <memory>

#include "linkit/linkit.h"

#include "vectra/core/gameobject.h"

#include "vectra/physics/rigidbody.h"
#include "vectra/physics/forces/simple_gravity.h"
#include "vectra/physics/forces/newtonian_gravity.h"
#include "vectra/physics/forces/anchored_spring.h"
#include "vectra/physics/forces/object_anchored_spring.h"


#include "vectra/rendering/light_source.h"
#include "vectra/rendering/camera.h"

/**
 * Helper function to emit a warning during scene loading.
 * Thread-safe: uses thread-local storage.
 */
static void emit_warning(const std::string& message)
{
    if (g_scene_load_warnings)
    {
        g_scene_load_warnings->push_back(message);
    }
}

/**
 * Helper function to generate an object name using model_name + counter.
 * Reuses Scene::add_game_object naming logic.
 */
static std::string generate_object_name(const std::string& model_name)
{
    if (g_name_counters)
    {
        int& counter = (*g_name_counters)[model_name];
        std::string name = model_name + "_" + std::to_string(counter);
        counter++;
        return name;
    }
    return model_name + "_0";
}

namespace linkit
{
    // Vector3
    inline void to_json(json &j, const linkit::Vector3 &v)
    {
        j = json::array({v.x, v.y, v.z});
    }

    inline void from_json(const json &j, linkit::Vector3 &v)
    {
        v.x = j.at(0).get<linkit::real>();
        v.y = j.at(1).get<linkit::real>();
        v.z = j.at(2).get<linkit::real>();
    }

    // Quaternion
    inline void to_json(json &j, const linkit::Quaternion &q)
    {
        j = json::array({q.w, q.x, q.y, q.z});
    }

    inline void from_json(const json &j, linkit::Quaternion &q)
    {
        if (j.size() == 4)
        {
            q.w = j.at(0).get<linkit::real>();
            q.x = j.at(1).get<linkit::real>();
            q.y = j.at(2).get<linkit::real>();
            q.z = j.at(3).get<linkit::real>();
        } else if (j.size() == 3)
        {
            // Handle Euler angles (pitch, yaw, roll)
            linkit::real pitch = j.at(0).get<linkit::real>();
            linkit::real yaw = j.at(1).get<linkit::real>();
            linkit::real roll = j.at(2).get<linkit::real>();

            linkit::real cy = linkit::real_cos(yaw * 0.5);
            linkit::real sy = linkit::real_sin(yaw * 0.5);
            linkit::real cp = linkit::real_cos(pitch * 0.5);
            linkit::real sp = linkit::real_sin(pitch * 0.5);
            linkit::real cr = linkit::real_cos(roll * 0.5);
            linkit::real sr = linkit::real_sin(roll * 0.5);

            q.w = cr * cp * cy + sr * sp * sy;
            q.x = sr * cp * cy - cr * sp * sy;
            q.y = cr * sp * cy + sr * cp * sy;
            q.z = cr * cp * sy - sr * sp * cy;
        } else
        {
            throw std::runtime_error("Invalid quaternion/euler angles format");
        }
    }
}

namespace glm
{
    // vec3
    inline void to_json(json &j, const glm::vec3 &v)
    {
        j = json::array({v.x, v.y, v.z});
    }

    inline void from_json(const json &j, glm::vec3 &v)
    {
        v.x = j.at(0).get<float>();
        v.y = j.at(1).get<float>();
        v.z = j.at(2).get<float>();
    }
}


// Transform
inline void to_json(json &j, const Transform &transform)
{
    j = json{
        {"position", transform.position},
        {"rotation", transform.rotation},
        {"scale", transform.scale}
    };
}

inline void from_json(const json &j, Transform &transform)
{
    // Default values: position [0,0,0], rotation [1,0,0,0] (identity), scale [1,1,1]
    if (j.contains("position"))
        j.at("position").get_to(transform.position);
    else
        transform.position = linkit::Vector3(0, 0, 0);

    if (j.contains("rotation"))
        j.at("rotation").get_to(transform.rotation);
    else
        transform.rotation = linkit::Quaternion(1, 0, 0, 0);

    if (j.contains("scale"))
        j.at("scale").get_to(transform.scale);
    else
        transform.scale = linkit::Vector3(1, 1, 1);
}

// Rigidbody
inline void to_json(json &j, const Rigidbody &rb)
{
    j = json{
        { "transform", rb.transform },
        {"mass", rb.mass},
        {"velocity", rb.velocity},
        {"angular_velocity", rb.angular_velocity},
        {"acceleration", rb.acceleration},
        {"angular_acceleration", rb.angular_acceleration}

    };
}

inline void from_json(const json &j, Rigidbody &rb)
{
    // Default values for rigidbody properties
    if (j.contains("transform"))
        j.at("transform").get_to(rb.transform);
    // else: transform uses its own defaults from Transform::from_json

    if (j.contains("mass"))
    {
        j.at("mass").get_to(rb.mass);
        rb.inverse_mass = (rb.mass != 0) ? (1.0 / rb.mass) : 0.0;
    }
    else
    {
        rb.mass = 0;
        rb.inverse_mass = 0;
    }

    if (j.contains("velocity"))
        j.at("velocity").get_to(rb.velocity);
    else
        rb.velocity = linkit::Vector3(0, 0, 0);

    if (j.contains("angular_velocity"))
        j.at("angular_velocity").get_to(rb.angular_velocity);
    else
        rb.angular_velocity = linkit::Vector3(0, 0, 0);

    if (j.contains("acceleration"))
        j.at("acceleration").get_to(rb.acceleration);
    else
        rb.acceleration = linkit::Vector3(0, 0, 0);

    if (j.contains("angular_acceleration"))
        j.at("angular_acceleration").get_to(rb.angular_acceleration);
    else
        rb.angular_acceleration = linkit::Vector3(0, 0, 0);
}

// Gameobject
inline void to_json(json &j, const GameObject &obj)
{
    j = json{
        {"name", obj.name},
        { "model_name", obj.model_name},
        {"rigidbody", obj.rb},
        {"collider_type", obj.collider->tag}
    };
}

inline void from_json(const json &j, GameObject &obj)
{
    // Default model_name is "cube"
    if (j.contains("model_name"))
        j.at("model_name").get_to(obj.model_name);
    else
        obj.model_name = "cube";

    // Auto-generate name if not provided (uses model_name + counter)
    if (j.contains("name"))
        j.at("name").get_to(obj.name);
    else
        obj.name = generate_object_name(obj.model_name);

    // Rigidbody with its own defaults
    if (j.contains("rigidbody"))
        j.at("rigidbody").get_to(obj.rb);
    // else: rigidbody uses its own defaults

    // Default collider is ColliderBox
    std::string collider_type = "ColliderBox";
    if (j.contains("collider_type"))
        j.at("collider_type").get_to(collider_type);
    obj.set_collider_type(collider_type);
}


// Camera
inline void to_json(json &j, const Camera &camera)
{
    j = json{
        {"fov", camera.fov},
        { "transform", camera.transform }
    };
}

inline void from_json(const json &j, Camera &camera)
{
    // Default fov is 90
    if (j.contains("fov"))
        j.at("fov").get_to(camera.fov);
    else
        camera.fov = 90.0f;

    // Transform with its own defaults
    if (j.contains("transform"))
        j.at("transform").get_to(camera.transform);
    // else: transform uses its own defaults
}



// LightSource
inline void to_json(json &j, const LightSource &light)
{
    j = json{
        {"colour", light.colour},
        {"position", light.position},
    };
}

inline void from_json(const json &j, LightSource &light)
{
    // Default colour is white [1,1,1]
    if (j.contains("colour"))
        j.at("colour").get_to(light.colour);
    else
        light.colour = glm::vec3(1.0f, 1.0f, 1.0f);

    // Default position is origin [0,0,0]
    if (j.contains("position"))
        j.at("position").get_to(light.position);
    else
        light.position = glm::vec3(0.0f, 0.0f, 0.0f);
}


// Cant use automatic to and from method as scene data is required to pass gameobjects as arguments

// SimpleGravity
void simple_gravity_to_json(json &j, const SimpleGravity &gravity)
{
    j = json{
            {"type", "simple_gravity"},
        {"field", gravity.gravitational_field}
    };
}

void simple_gravity_from_json(const json &j, SimpleGravity &gravity)
{
    // Default field is [0,0,0]
    if (j.contains("field"))
        j.at("field").get_to(gravity.gravitational_field);
    else
        gravity.gravitational_field = linkit::Vector3(0, 0, 0);
}

// NewtonianGravity
void newtonian_gravity_to_json(json &j, const NewtonianGravity &gravity, const Scene &scene)
{
    std::vector<int> affect_object_indices;
    for (int i=0; i < scene.game_objects.size(); ++i)
    {
        const auto& obj = scene.game_objects[i];
        for (const auto& affected_obj : gravity.affected_objects)
        {
            if (&obj == &affected_obj)
            {
                affect_object_indices.push_back(i);
                break;
            }
        }
    }
    j = json{
        {"type", "newtonian_gravity"},
        {"gravitational_constant", gravity.gravitational_constant},
        {"affected_object_indices", affect_object_indices}
    };
}

/**
 * Deserialize NewtonianGravity from JSON.
 * Returns false if the force should be skipped (no valid affected objects).
 */
bool newtonian_gravity_from_json(const json &j, NewtonianGravity &gravity, const Scene &scene, const std::string& object_name)
{
    // Default gravitational constant is 0
    if (j.contains("gravitational_constant"))
        j.at("gravitational_constant").get_to(gravity.gravitational_constant);
    else
        gravity.gravitational_constant = 0;

    if (!j.contains("affected_object_indices"))
    {
        emit_warning("Warning: newtonian_gravity on object '" + object_name + "' has no affected_object_indices, skipping");
        return false;
    }

    const auto& indices_json = j.at("affected_object_indices");
    for (const auto& index_json : indices_json)
    {
        int index = index_json.get<int>();
        if (index >= 0 && index < static_cast<int>(scene.game_objects.size()))
        {
            gravity.affected_objects.push_back(scene.game_objects[index]);
        }
        else
        {
            emit_warning("Warning: Invalid object index " + std::to_string(index) + " for newtonian_gravity on object '" + object_name + "', ignoring index");
        }
    }

    if (gravity.affected_objects.empty())
    {
        emit_warning("Warning: newtonian_gravity on object '" + object_name + "' has no valid affected objects, skipping");
        return false;
    }

    return true;
}

// AnchoredSpring
void anchored_spring_to_json(json &j, const AnchoredSpring &spring)
{
    j = json{
        {"type", "anchored_spring"},
        {"anchor_point", spring.anchor_point},
        {"spring_constant", spring.spring_constant},
        {"rest_length", spring.rest_length},
        {"damping", spring.damping}
    };
}

void anchored_spring_from_json(const json &j, AnchoredSpring &spring)
{
    // Default anchor_point is [0,0,0]
    if (j.contains("anchor_point"))
        j.at("anchor_point").get_to(spring.anchor_point);
    else
        spring.anchor_point = linkit::Vector3(0, 0, 0);

    // Default spring_constant is 0
    if (j.contains("spring_constant"))
        j.at("spring_constant").get_to(spring.spring_constant);
    else
        spring.spring_constant = 0;

    // Default rest_length is 0
    if (j.contains("rest_length"))
        j.at("rest_length").get_to(spring.rest_length);
    else
        spring.rest_length = 0;

    // Default damping is 0
    if (j.contains("damping"))
        j.at("damping").get_to(spring.damping);
    else
        spring.damping = 0;
}

// ObjectAnchoredSpring
void object_anchored_spring_to_json(json &j, const ObjectAnchoredSpring &spring, const Scene &scene)
{
    int object_index = -1;
    for (int i=0; i < scene.game_objects.size(); ++i)
    {
        if (&scene.game_objects[i] == spring.anchor_object)
        {
            object_index = i;
            break;
        }
    }
    j = json{
        {"type", "object_anchored_spring"},
        {"object_index", object_index},
        {"spring_constant", spring.spring_constant},
        {"rest_length", spring.rest_length},
        {"damping", spring.damping}
    };
}

/**
 * Deserialize ObjectAnchoredSpring from JSON.
 * Returns false if the force should be skipped (invalid anchor object).
 */
bool object_anchored_spring_from_json(const json &j, ObjectAnchoredSpring &spring, const Scene &scene, const std::string& object_name)
{
    // object_index is required
    if (!j.contains("object_index"))
    {
        emit_warning("Warning: object_anchored_spring on object '" + object_name + "' has no object_index, skipping");
        return false;
    }

    int object_index;
    j.at("object_index").get_to(object_index);
    if (object_index >= 0 && object_index < static_cast<int>(scene.game_objects.size()))
    {
        spring.anchor_object = const_cast<GameObject*>(&scene.game_objects[object_index]);
    }
    else
    {
        emit_warning("Warning: object_anchored_spring on object '" + object_name + "' has invalid anchor index " + std::to_string(object_index) + ", skipping");
        return false;
    }

    // Default spring_constant is 0
    if (j.contains("spring_constant"))
        j.at("spring_constant").get_to(spring.spring_constant);
    else
        spring.spring_constant = 0;

    // Default rest_length is 0
    if (j.contains("rest_length"))
        j.at("rest_length").get_to(spring.rest_length);
    else
        spring.rest_length = 0;

    // Default damping is 0
    if (j.contains("damping"))
        j.at("damping").get_to(spring.damping);
    else
        spring.damping = 0;

    return true;
}



// Scene
inline void to_json(json &j, const Scene &scene)
{
    // Placeholder: proper serialization to implement later
    j = json{
        {"name", scene.name},
        {"camera", scene.camera},
        {"objects", json::array()},
        {"lights", json::array()}
    };
    for (const auto& obj : scene.game_objects)
    {
        json obj_json;
        to_json(obj_json, obj);

        // Serialize force generators
        obj_json["force_generators"] = json::array();
        for (const auto& reg : scene.force_registry.registered_forces)
        {
            if (reg.obj == &obj)
            {
                json fg_json;
                if (auto* simple_gravity = dynamic_cast<SimpleGravity*>(reg.force_generator.get()))
                {
                    simple_gravity_to_json(fg_json, *simple_gravity);
                }
                else if (auto* newtonian_gravity = dynamic_cast<NewtonianGravity*>(reg.force_generator.get()))
                {
                    newtonian_gravity_to_json(fg_json, *newtonian_gravity, scene);
                }
                else if (auto* anchored_spring = dynamic_cast<AnchoredSpring*>(reg.force_generator.get()))
                {
                    anchored_spring_to_json(fg_json, *anchored_spring);
                }
                else if (auto* object_anchored_spring = dynamic_cast<ObjectAnchoredSpring*>(reg.force_generator.get()))
                {
                    object_anchored_spring_to_json(fg_json, *object_anchored_spring, scene);
                }
                obj_json["force_generators"].push_back(fg_json);
            }
        }

        j["objects"].push_back(obj_json);
    }

    for (const auto& light : scene.light_sources)
    {
        json light_json;
        to_json(light_json, light);
        j["lights"].push_back(light_json);
    }
}

inline void from_json(const json &j, Scene &scene)
{
    // Default name is "New Scene"
    if (j.contains("name"))
        j.at("name").get_to(scene.name);
    else
        scene.name = "New Scene";

    // Camera with its own defaults
    if (j.contains("camera"))
        j.at("camera").get_to(scene.camera);
    // else: camera uses its own defaults

    // Objects array (empty by default)
    if (j.contains("objects"))
    {
        const auto& objects_json = j.at("objects");
        for (const auto& obj_json : objects_json)
        {
            GameObject obj;
            obj_json.get_to(obj);
            scene.add_game_object(std::move(obj));

            // Force generators (empty by default)
            if (obj_json.contains("force_generators"))
            {
                const auto& force_generators_json = obj_json.at("force_generators");
                const std::string& obj_name = scene.game_objects.back().name;

                for (const auto& fg_json : force_generators_json)
                {
                    // Type is required for force generators
                    if (!fg_json.contains("type"))
                    {
                        emit_warning("Warning: Force generator on object '" + obj_name + "' has no type, skipping");
                        continue;
                    }

                    std::string type = fg_json.at("type").get<std::string>();

                    if (type == "simple_gravity")
                    {
                        // Set the default value to be overridden by json
                        auto gravity = std::make_shared<SimpleGravity>(0);
                        simple_gravity_from_json(fg_json, *gravity);
                        scene.force_registry.add(&scene.game_objects.back(), gravity);
                    }
                    else if (type == "newtonian_gravity")
                    {
                        // Empty placeholder
                        std::vector<GameObject> game_objects;
                        auto gravity = std::make_shared<NewtonianGravity>(game_objects, 0);
                        if (newtonian_gravity_from_json(fg_json, *gravity, scene, obj_name))
                        {
                            scene.force_registry.add(&scene.game_objects.back(), gravity);
                        }
                    }
                    else if (type == "anchored_spring")
                    {
                        auto spring = std::make_shared<AnchoredSpring>(linkit::Vector3(0,0,0), 0, 0, 0);
                        anchored_spring_from_json(fg_json, *spring);
                        scene.force_registry.add(&scene.game_objects.back(), spring);
                    }
                    else if (type == "object_anchored_spring")
                    {
                        auto spring = std::make_shared<ObjectAnchoredSpring>(nullptr, 0, 0, 0);
                        if (object_anchored_spring_from_json(fg_json, *spring, scene, obj_name))
                        {
                            scene.force_registry.add(&scene.game_objects.back(), spring);
                        }
                    }
                    else
                    {
                        emit_warning("Warning: Unknown force type '" + type + "' on object '" + obj_name + "', skipping");
                    }
                }
            }
        }
    }

    // Lights array (empty by default)
    if (j.contains("lights"))
    {
        const auto& lights_json = j.at("lights");
        for (const auto& light_json : lights_json)
        {
            LightSource light;
            light_json.get_to(light);
            scene.add_light_source(std::move(light));
        }
    }
}

// Public wrappers
void SceneSerializer::serialize_scene(const Scene& scene, const std::string& filename)
{

    std::filesystem::path scenes_dir = get_scenes_directory();
    std::filesystem::path file_path = scenes_dir / filename;
    json scene_json = scene;
    write_json(file_path.string(), scene_json);

}

SceneLoadResult SceneSerializer::deserialize_scene(const std::string& filename)
{
    SceneLoadResult result;

    // Set up thread-local storage for warnings and name counters
    g_scene_load_warnings = &result.warnings;
    std::unordered_map<std::string, int> name_counters;
    g_name_counters = &name_counters;

    try
    {
        auto scene_data = read_json(filename);
        result.scene = scene_data.get<Scene>();
    }
    catch (const json::parse_error& e)
    {
        result.errors.push_back("Error: JSON parse error in file '" + filename + "': " + e.what());
    }
    catch (const json::type_error& e)
    {
        result.errors.push_back("Error: JSON type error in file '" + filename + "': " + e.what());
    }
    catch (const std::runtime_error& e)
    {
        result.errors.push_back("Error: " + std::string(e.what()));
    }
    catch (const std::exception& e)
    {
        result.errors.push_back("Error: Unexpected error loading '" + filename + "': " + e.what());
    }

    // Clear thread-local storage
    g_scene_load_warnings = nullptr;
    g_name_counters = nullptr;

    return result;
}

// Private helpers
std::filesystem::path SceneSerializer::get_scenes_directory()
{
    // Try the current directory first (for production builds)
    std::filesystem::path current_scenes = std::filesystem::current_path() / "resources/scenes";
    if (std::filesystem::exists(current_scenes))
    {
        return current_scenes;
    }

    // Fallback: parent directory (for IDE builds like cmake-build-debug)
    std::filesystem::path parent_scenes = std::filesystem::current_path().parent_path() / "resources/scenes";
    if (std::filesystem::exists(parent_scenes))
    {
        return parent_scenes;
    }

    throw std::runtime_error("Could not locate scenes directory");
}

void SceneSerializer::print_json(const json& j) const
{
    std::cout << j.dump(indentation_level_) << std::endl;
}

json SceneSerializer::read_json(const std::string& filename)
{
    std::filesystem::path scenes_dir = get_scenes_directory();
    std::filesystem::path file_path = scenes_dir / filename;
    std::ifstream f(file_path);
    if (!f.is_open())
    {
        throw std::runtime_error("Could not open file: " + file_path.string());
    }
    json data = json::parse(f);
    return data;
}

void SceneSerializer::write_json(const std::string& filename, const json& j)
{
    std::filesystem::path scenes_dir = get_scenes_directory();
    std::filesystem::path file_path = scenes_dir / filename;
    std::ofstream f(file_path);
    if (!f.is_open())
    {
        throw std::runtime_error("Could not open file for writing: " + file_path.string());
    }
    f << j.dump(indentation_level_);
}


