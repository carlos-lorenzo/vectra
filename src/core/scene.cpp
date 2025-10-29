#include <vector>
#include "vectra/core/scene.h"
#include "vectra/rendering/light_source.h"
#include "vectra/rendering/camera.h"

Scene::Scene()
{
    game_objects = std::vector<GameObject>();
    light_sources = std::vector<LightSource>();
    camera = Camera();
}

void Scene::add_game_object(const GameObject& obj)
{
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
        obj.rb.step(dt);
    }
}
