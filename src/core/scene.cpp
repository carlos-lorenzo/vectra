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
    bvh_root = nullptr;
    bvh_node_map = std::unordered_map<GameObject*, BVHNode<BoundingSphere>*>();
}

void Scene::add_game_object(GameObject& obj)
{
    obj.rb.set_inverse_inertia_tensor(obj.rb.cuboid_inertia_tensor());
    game_objects.push_back(obj);
    GameObject* new_obj_ptr = &game_objects.back();
    // Fixed radius for now, will be set dynamically based on mesh
    linkit::real radius = obj.rb.transform.scale.magnitude();
    BoundingSphere new_volume(obj.rb.transform.position, radius);
    if (!bvh_root)
    {
        bvh_root = std::make_unique<BVHNode<BoundingSphere>>(nullptr, new_volume, new_obj_ptr);
        bvh_node_map[new_obj_ptr] = bvh_root.get();
    }
    else
    {
        BVHNode<BoundingSphere>* new_node = bvh_root->insert(new_obj_ptr, new_volume);
        bvh_node_map[new_obj_ptr] = new_node;
    }


}

void Scene::update_bvh()
{
    for (auto& obj : game_objects)
    {
        // This check assumes the rigid body has a way to know if it moved.
        // A simple flag set during integration would work.
        if (!obj.rb.has_moved) continue;

        auto it = bvh_node_map.find(&obj);
        if (it != bvh_node_map.end())
        {
            std::cout << "updated" << std::endl;
            BVHNode<BoundingSphere>* node = it->second;
            // Update the node's bounding volume position and size if needed
            node->bounding_volume->center = obj.rb.transform.position;
            // node->bounding_volume->radius = ...; // if radius can change

            // Re-insert the node to maintain tree quality.
            // A more advanced approach could be to just refit the bounds upwards
            // and only re-insert if it has moved significantly.
            node->recalc_upwards();
            std::cout << node->bounding_volume->center.to_string() << std::endl;
        }
    }
}


void Scene::add_light_source(const LightSource& obj)
{
    light_sources.push_back(obj);
}



void Scene::step(const linkit::real dt)
{
    update_bvh();
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
