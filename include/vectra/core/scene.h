#ifndef VECTRA_SCENE_H
#define VECTRA_SCENE_H


#include <vector>
#include <memory>
#include <unordered_map>

#include "vectra/rendering/camera.h"
#include "vectra/rendering/light_source.h"
#include "vectra/rendering/skybox.h"
#include "vectra/core/gameobject.h"
#include "vectra/physics/force_registry.h"
#include "vectra/physics/BVHNode.h"
#include "vectra/physics/bounding_volumes/bounding_sphere.h"

class Scene
{
    public:
        Scene();
        std::vector<GameObject> game_objects;
        std::vector<LightSource> light_sources;
        Camera camera;
        Skybox skybox;
        ForceRegistry force_registry;
        std::unique_ptr<BVHNode<BoundingSphere>> bvh_root;
private:
    std::unordered_map<GameObject*, BVHNode<BoundingSphere>*> bvh_node_map;


public:
    void add_game_object(GameObject& obj);
    void add_light_source(const LightSource& obj);
    void step(linkit::real dt);

private:
    void update_bvh();
};
#endif //VECTRA_SCENE_H