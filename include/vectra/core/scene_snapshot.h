#ifndef VECTRA_SCENE_SNAPSHOT_H
#define VECTRA_SCENE_SNAPSHOT_H

#include <vector>

#include "vectra/core/gameobject_snapshot.h"

struct SceneSnapshot
{
    std::vector<GameObjectSnapshot> object_snapshots;


};
#endif //VECTRA_SCENE_SNAPSHOT_H