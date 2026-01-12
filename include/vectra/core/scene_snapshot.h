#ifndef VECTRA_SCENE_SNAPSHOT_H
#define VECTRA_SCENE_SNAPSHOT_H

#include <vector>

#include "vectra/core/gameobject.h"
#include "vectra/physics/transform.h"
#include "vectra/rendering/light_source.h"

struct SceneSnapshot
{
    std::vector<GameObject> objects; // Will be removed in the future as redundant data stored
    std::vector<Transform> object_transforms;
    std::vector<LightSource> light_sources;

};
#endif //VECTRA_SCENE_SNAPSHOT_H