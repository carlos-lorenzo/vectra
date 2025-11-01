#include <vector>
#include "vectra/core/scene.h"

#include <iostream>
#include <ostream>

#include "vectra/rendering/light_source.h"
#include "vectra/rendering/camera.h"


Scene::Scene()
{
    game_objects = std::vector<GameObject>();
    light_sources = std::vector<LightSource>();
    camera = Camera();
    force_registry = ForceRegistry();
    skybox = Skybox();
}

void Scene::add_game_object(GameObject& obj)
{
    obj.rb.set_inverse_inertia_tensor(obj.rb.cuboid_inertia_tensor());
    game_objects.push_back(obj);
}

void Scene::add_light_source(const LightSource& obj)
{
    light_sources.push_back(obj);
}



void Scene::step(const linkit::real dt)
{
    for (auto& obj : game_objects)
    {
        obj.rb.clear_accumulators();
    }
    force_registry.update_forces(dt);


    for (auto& obj : game_objects)
    {
        obj.rb.step(dt);
    }
}
