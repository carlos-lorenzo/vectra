#ifndef VECTRA_OBJECT_SNAPSHOT_H
#define VECTRA_OBJECT_SNAPSHOT_H

#include <string>

#include "vectra/physics/transform.h"

struct GameObjectSnapshot
{
    std::string model_name;
    Transform transform;
};

#endif //VECTRA_OBJECT_SNAPSHOT_H