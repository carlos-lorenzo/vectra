#include "vectra/core/gameobject.h"
#include "vectra/rendering/mesh.h"

GameObject::GameObject() : model("resources/models/primitives/cube.obj", false)
{
    rb = Rigidbody();
    shader = Shader("resources/shaders/model.vert", "resources/shaders/phong.frag");
}
