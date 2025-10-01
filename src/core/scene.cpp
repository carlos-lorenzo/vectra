#include <vector>
#include "vectra/core/scene.h"

Scene::Scene()
{
    game_objects = std::vector<GameObject>();
    camera = Camera();

}

void Scene::add_game_object(const GameObject& obj)
{
    game_objects.push_back(obj);
}

void Scene::step(const linkit::real dt)
{
    for (auto& obj : game_objects)
    {
        obj.rb.step(dt);
    }
}
