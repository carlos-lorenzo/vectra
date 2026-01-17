#ifndef VECTRA_SCENE_H
#define VECTRA_SCENE_H


#include <vector>
#include <deque>
#include <memory>
#include <unordered_map>

#include "vectra/rendering/camera.h"
#include "vectra/rendering/light_source.h"
#include "vectra/rendering/skybox.h"
#include "vectra/core/gameobject.h"
#include "vectra/core/scene_snapshot.h"
#include "vectra/physics/force_registry.h"
#include "vectra/physics/BVHNode.h"
#include "vectra/physics/bounding_volumes/bounding_sphere.h"
#include "vectra/physics/collision_handler.h"

class Scene
{
    public:
        Scene();
        void rebuild_bvh_node_map();
        std::string name;
        std::deque<GameObject> game_objects; // stable pointers
        std::vector<LightSource> light_sources;
        Camera camera;
        Skybox skybox;
        ForceRegistry force_registry;
        std::unique_ptr<BVHNode<BoundingSphere>> bvh_root;
        CollisionHandler collision_handler;
private:
    std::unordered_map<GameObject*, BVHNode<BoundingSphere>*> bvh_node_map;
    std::unordered_map<std::string, int> name_counters_; // For auto-generating object names


public:
    void add_game_object(GameObject obj);
    void add_light_source(const LightSource& obj);
    void step(linkit::real dt);

    SceneSnapshot create_snapshot() const;

private:
    void update_bvh();
};
#endif //VECTRA_SCENE_H