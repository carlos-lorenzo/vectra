#ifndef VECTRA_SCENE_LIGHTS_H
#define VECTRA_SCENE_LIGHTS_H

#include <vector>

#include "vectra/rendering/shader.h"
#include "vectra/rendering/light_sources.h"

struct SceneLights
{
    std::vector<DirectionalLight> directional_lights;
    std::vector<PointLight> point_lights;
    std::vector<SpotLight> spot_lights;

    SceneLights() : directional_lights(), point_lights(), spot_lights()
    {
    }

    void clear()
    {
        directional_lights.clear();
        point_lights.clear();
        spot_lights.clear();
    }

    void add_directional_light(const DirectionalLight& light)
    {
        directional_lights.push_back(light);
    }

    void add_point_light(const PointLight& light)
    {
        point_lights.push_back(light);
    }

    void add_spot_light(const SpotLight& light)
    {
        spot_lights.push_back(light);
    }

    void setup_lighting(const Shader& shader, glm::vec3 camera_position) const
    {
        shader.use();
        shader.set_vec3("view_pos", camera_position);

        // Directional Lights
        shader.set_int("num_directional_lights", static_cast<int>(directional_lights.size()));
        for (size_t i = 0; i < directional_lights.size(); ++i) {
            directional_lights[i].apply_to_shader(shader, "directional_lights[" + std::to_string(i) + "]");
        }

        // Point Lights
        shader.set_int("num_point_lights", static_cast<int>(point_lights.size()));
        for (size_t i = 0; i < point_lights.size(); ++i) {
            point_lights[i].apply_to_shader(shader, "point_lights[" + std::to_string(i) + "]");
        }

        // Spotlights
        shader.set_int("num_spot_lights", static_cast<int>(spot_lights.size()));
        for (size_t i = 0; i < spot_lights.size(); ++i) {
            spot_lights[i].apply_to_shader(shader, "spot_lights[" + std::to_string(i) + "]");
        }
    }
};

#endif //VECTRA_SCENE_LIGHTS_H