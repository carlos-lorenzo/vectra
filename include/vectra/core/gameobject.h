//
// Created by carlos on 9/29/25.
//

#ifndef VECTRA_GAMEOBJECT_H
#define VECTRA_GAMEOBJECT_H

#include "vectra/rendering/mesh.h"
#include "vectra/physics/rigidbody.h"
#include "vectra/physics/transform.h"

class GameObject
{
    public:
        Rigidbody rb;
        Mesh mesh;

    GameObject();
};
#endif //VECTRA_GAMEOBJECT_H