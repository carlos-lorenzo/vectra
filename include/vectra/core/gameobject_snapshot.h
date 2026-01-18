#ifndef VECTRA_OBJECT_SNAPSHOT_H
#define VECTRA_OBJECT_SNAPSHOT_H

#include <string>

#include "vectra/physics/transform.h"

struct GameObjectSnapshot
{
    std::string name;       // Display name for hierarchy
    std::string model_name;
    Transform transform;
    linkit::Vector3 force;
    bool has_spring;
    linkit::Vector3 spring_anchor;
};

#endif //VECTRA_OBJECT_SNAPSHOT_H