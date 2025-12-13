//
// Created by carlos on 9/29/25.
//

#ifndef VECTRA_TRANSFORM_H
#define VECTRA_TRANSFORM_H
#include <linkit/linkit.h>
#include "linkit/quaternion.h"

class Transform
{
    public:
        linkit::Vector3 position;
        linkit::Quaternion rotation;
        linkit::Vector3 scale;


        Transform();
        [[nodiscard]] linkit::Vector3 forward() const;
        [[nodiscard]] linkit::Vector3 up_dir() const;
        [[nodiscard]] linkit::Vector3 right_dir() const;
        Transform(const linkit::Vector3& init_position, const linkit::Quaternion& init_rotation, const linkit::Vector3& init_scale);
        [[nodiscard]] linkit::real size() const;

        void translate(const linkit::Vector3& delta);
        // Returns the Model Matrix (Translation * Rotation * Scale)
        [[nodiscard]] linkit::Matrix4 get_model_matrix() const;
};

#endif //VECTRA_TRANSFORM_H