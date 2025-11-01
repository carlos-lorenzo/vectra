//
// Created by carlos on 9/29/25.
//

#ifndef VECTRA_TRANSFORM_H
#define VECTRA_TRANSFORM_H
#include <linkit/vector3.h>
#include "linkit/quaternion.h"

class Transform
{
    public:
        linkit::Vector3 position;
        linkit::Quaternion rotation;
        linkit::Vector3 scale;


        Transform();
        linkit::Vector3 forward() const;
        linkit::Vector3 up_dir() const;
        linkit::Vector3 right_dir() const;
        Transform(const linkit::Vector3& init_position, const linkit::Quaternion& init_rotation, const linkit::Vector3& init_scale);

        void translate(const linkit::Vector3& delta);
};

#endif //VECTRA_TRANSFORM_H