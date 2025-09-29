//
// Created by carlos on 9/29/25.
//

#ifndef VECTRA_TRANSFORM_H
#define VECTRA_TRANSFORM_H
#include <linkit/vector3.h>

class Transform
{
    public:
        linkit::Vector3 position;
        linkit::Vector3 rotation;
        linkit::Vector3 scale;


        Transform();
        Transform(linkit::Vector3 init_position, linkit::Vector3 init_rotation, linkit::Vector3 init_scale);

        void translate(const linkit::Vector3& delta);
};

#endif //VECTRA_TRANSFORM_H