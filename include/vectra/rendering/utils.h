#ifndef VECTRA_UTILS_H
#define VECTRA_UTILS_H

#include <glm/glm.hpp>
#include <linkit/linkit.h>

inline glm::vec3 vector3_to_vec3(const linkit::Vector3 &v)
{
    return glm::vec3(v.x, v.y, v.z);
}

#endif //VECTRA_UTILS_H