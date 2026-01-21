#ifndef VECTRA_SCENE_H
#define VECTRA_SCENE_H


#include <deque>
#include <memory>
#include <unordered_map>

#include "vectra/rendering/camera.h"
#include "vectra/rendering/scene_lights.h"
#include "vectra/rendering/skybox.h"

#include "vectra/core/gameobject.h"
#include "vectra/core/scene_snapshot.h"
#include "vectra/core/engine_state.h"

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
        SceneLights scene_lights;
        Camera camera;
        Skybox skybox;
        ForceRegistry force_registry;
        std::unique_ptr<BVHNode<BoundingSphere>> bvh_root;
        CollisionHandler collision_handler;
private:
    std::unordered_map<GameObject*, BVHNode<BoundingSphere>*> bvh_node_map;
    std::unordered_map<std::string, int> name_counters_; // For auto-generating object names
    int max_collision_contacts_ = 1000;


public:
    void add_game_object(GameObject obj);
    void add_directional_light(const DirectionalLight& light);
    void add_point_light(const PointLight& light);
    void add_spot_light(const SpotLight& light);
    void step(linkit::real dt);
    void set_from_engine_state(const EngineState& state);

    SceneSnapshot create_snapshot() const;

private:
    void update_bvh();
};
#endif //VECTRA_SCENE_H

