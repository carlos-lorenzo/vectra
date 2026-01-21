#ifndef VECTRA_SCENE_SNAPSHOT_H
#define VECTRA_SCENE_SNAPSHOT_H

#include <vector>

#include "vectra/core/gameobject_snapshot.h"

#include "vectra/physics/BVHNode.h"
#include "vectra/physics/bounding_volumes/bounding_sphere.h"

struct SceneSnapshot
{
    std::vector<GameObjectSnapshot> object_snapshots;
    BVHNode<BoundingSphere>* bvh_root = nullptr;

};
#endif //VECTRA_SCENE_SNAPSHOT_H