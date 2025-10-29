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

        linkit::Vector3 angular_velocity;
        linkit::Vector3 angular_acceleration;

        linkit::Vector3 accumulated_force;
        linkit::Vector3 accumulated_torque;

        linkit::real mass;
        linkit::real inverse_mass;

        linkit::real linear_damping;

        Rigidbody();
        void step_rotation(linkit::real dt);
        void step_position(linkit::real dt);
        void step(linkit::real dt);

};


#endif //VECTRA_RIGIDBODY_H