//
// Created by carlos on 9/29/25.
//

#ifndef VECTRA_RIGIDBODY_H
#define VECTRA_RIGIDBODY_H
#include "transform.h"
#include "linkit/linkit.h"


class Rigidbody
{
    public:
        Transform transform;
        linkit::Vector3 velocity;
        linkit::Vector3 acceleration;
        linkit::real mass;
        Rigidbody();
        void step(linkit::real dt);


};


#endif //VECTRA_RIGIDBODY_H