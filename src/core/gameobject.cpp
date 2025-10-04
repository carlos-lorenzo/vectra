#include "vectra/core/gameobject.h"
#include "vectra/rendering/mesh.h"

GameObject::GameObject() : model("resources/models/cube/HydraMoonSimpleCube.fbx", true)
{
    rb = Rigidbody();
    shader = Shader("resources/shaders/model.vert", "resources/shaders/model.frag");
}
