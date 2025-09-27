//
// Created by carlos on 9/27/25.
//

#ifndef VECTRA_PARTICLE_H
#define VECTRA_PARTICLE_H
#include "linkit/precision.h"
#include "linkit/vector3.h"

namespace vectra
{
    class particle
    {
        linkit::Vector3 position;
        linkit::Vector3 velocity;
        linkit::Vector3 acceleration;
        linkit::real mass;
    public:
        particle() : position(0,0,0), velocity(0,0,0), acceleration(0,0,0), mass(1) {}
        particle(const linkit::Vector3& pos, const linkit::Vector3& vel, const linkit::Vector3& acc, linkit::real m)
            : position(pos), velocity(vel), acceleration(acc), mass(m) {}

        void update(const linkit::real dt)
        {
            position += velocity * dt + 0.5 * acceleration * dt * dt;
            velocity += acceleration * dt;
        }
    };
} // vectra

#endif //VECTRA_PARTICLE_H