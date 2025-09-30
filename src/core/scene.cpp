#include <vector>
#include "vectra/core/scene.h"

Scene::Scene()
{
    game_objects = std::vector<GameObject>();
}

void Scene::add_game_object(const GameObject& obj)
{
    game_objects.push_back(obj);
}
