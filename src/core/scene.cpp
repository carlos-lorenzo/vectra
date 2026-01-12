#include <vector>
#include <deque>
#include "vectra/core/scene.h"

#include <iostream>
#include <ostream>

#include "vectra/rendering/light_source.h"
#include "vectra/rendering/camera.h"

#include "vectra/physics/BVHNode.h"


Scene::Scene()
{
    game_objects = std::deque<GameObject>();
    light_sources = std::vector<LightSource>();
    camera = Camera();
    force_registry = ForceRegistry();
    skybox = Skybox();
    bvh_root = nullptr;
    bvh_node_map = std::unordered_map<GameObject*, BVHNode<BoundingSphere>*>();
    collision_handler = CollisionHandler();
}

// In Scene class (private):
void Scene::rebuild_bvh_node_map()
{
    bvh_node_map.clear();
    if (!bvh_root) return;
    std::vector<BVHNode<BoundingSphere>*> stack{ bvh_root.get() };
    while (!stack.empty())
    {
        auto* n = stack.back(); stack.pop_back();
        if (!n) continue;
        if (n->is_leaf() && n->object)
            bvh_node_map[n->object] = n;
        else
        {
            stack.push_back(n->children[0]);
            stack.push_back(n->children[1]);
        }
    }
}

// add_game_object
void Scene::add_game_object(GameObject obj)
{
    if (obj.get_collider().tag == "ColliderBox")
    {
        obj.rb.set_inverse_inertia_tensor(obj.rb.cuboid_inertia_tensor());
    }
    else if (obj.get_collider().tag == "ColliderSphere")
    {
        obj.rb.set_inverse_inertia_tensor(obj.rb.sphere_inertia_tensor());
    }
    else
    {
        obj.rb.set_inverse_inertia_tensor(obj.rb.cuboid_inertia_tensor());
    }

    // Compute bounding info before moving the object
    linkit::real radius = obj.rb.transform.scale.magnitude();
    auto position = obj.rb.transform.position;

    game_objects.push_back(std::move(obj));

    GameObject* new_obj_ptr = &game_objects.back();

    // FIX: Update the collider to point to the transform of the object in its new memory location
    new_obj_ptr->get_collider().set_transform(&new_obj_ptr->rb.transform);

    BoundingSphere new_volume(position, radius);

    BVHNode<BoundingSphere>* leaf = nullptr;
    if (!bvh_root)
    {
        bvh_root = std::make_unique<BVHNode<BoundingSphere>>(nullptr, new_volume, new_obj_ptr);
        leaf = bvh_root.get();
    }
    else
    {
        leaf = bvh_root->insert(new_obj_ptr, new_volume); // now returns the leaf
    }

    // Rebuild map so the old object (split into children[0]) is re-mapped correctly
    rebuild_bvh_node_map();
}

// update_bvh
void Scene::update_bvh()
{
    if (!bvh_root) return;

    for (auto& obj : game_objects)
    {
        if (!obj.rb.has_moved) continue;

        auto it = bvh_node_map.find(&obj);
        if (it == bvh_node_map.end() || !it->second) continue;

        auto* node = it->second; // this is the leaf
        // Update leaf’s volume and refit upwards
        node->bounding_volume->center = obj.rb.transform.position;
        // node->bounding_volume->radius = ...; // if radius may change
        node->recalc_upwards();
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

    unsigned int limit = 500; // Max number of potential contacts to consider. Will be passed from engine settings later.
    std::vector<PotentialContact> possible_contacts;
    possible_contacts = bvh_root->potential_contacts_inside(possible_contacts, limit);
    collision_handler.narrow_phase(possible_contacts);
    collision_handler.solve_contacts();
    collision_handler.resolve_interpretations();


    collision_handler.clear_contacts();
}

SceneSnapshot Scene::create_snapshot() const
{
    SceneSnapshot snapshot;
    for (const auto& obj : game_objects)
    {
        snapshot.objects.push_back(obj);
        snapshot.object_transforms.push_back(obj.rb.transform);
    }

    for (const auto& light : light_sources)
    {
        snapshot.light_sources.push_back(light);
    }

    return snapshot;
}