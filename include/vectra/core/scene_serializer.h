#ifndef VECTRA_SCENE_SERIALIZER_H
#define VECTRA_SCENE_SERIALIZER_H

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "vectra/core/scene.h"


using json = nlohmann::json;

/**
 * Result of deserializing a scene, containing the scene and any warnings/errors.
 */
struct SceneLoadResult
{
    Scene scene;
    std::vector<std::string> warnings;  // Non-fatal issues (unknown force, invalid index, etc.)
    std::vector<std::string> errors;    // Fatal issues (JSON syntax error, file not found, etc.)

    bool has_errors() const { return !errors.empty(); }
    bool has_warnings() const { return !warnings.empty(); }
};

/**
 * Thread-local storage for collecting warnings during JSON deserialization.
 * Used because nlohmann::json from_json signatures cannot be modified.
 */
inline thread_local std::vector<std::string>* g_scene_load_warnings = nullptr;

/**
 * Thread-local name counters for auto-generating object names during deserialization.
 */
inline thread_local std::unordered_map<std::string, int>* g_name_counters = nullptr;

class SceneSerializer
{

public:
    void serialize_scene(const Scene& scene, const std::string& filename);
    SceneLoadResult deserialize_scene(const std::string& filename);

private:
    int indentation_level_ = 4;
    json read_json(const std::string& filename);
    static std::filesystem::path get_scenes_directory();
    void print_json(const json& j) const;
    void write_json(const std::string& filename, const json& j);

};

#endif //VECTRA_SCENE_SERIALIZER_H