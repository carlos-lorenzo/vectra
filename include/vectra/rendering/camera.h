#ifndef VECTRA_CAMERA_H
#define VECTRA_CAMERA_H

#include "vectra/core/gameobject.h"
#include "vectra/physics/transform.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
    public:
        Transform transform;


        linkit::real fov;
        linkit::real aspectRatio;
        linkit::real nearPlane;
        linkit::real farPlane;

        linkit::real movement_speed;
        linkit::real mouse_sensitivity;
        linkit::real max_pitch;

        Camera();
        [[nodiscard]] glm::mat4 get_view_matrix();
        [[nodiscard]] glm::mat4 get_projection_matrix() const;
        static glm::mat4 get_model_matrix(const GameObject &obj);
};
#endif //VECTRA_CAMERA_H