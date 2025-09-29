//
// Created by carlos on 9/29/25.
//

#include "../../include/vectra/rendering/mesh.h"

Mesh::Mesh(Transform* parentTransform)
{
    pParentTransform = parentTransform;

}

Mesh::Mesh()
{
    pParentTransform = nullptr;
    vertices[0] = vertices[1] = vertices[2] = 0.0f;
    indices[0] = indices[1] = indices[2] = 0;
}

void Mesh::construct_vertices()
{
    vertices[0] = pParentTransform->position.x + pParentTransform->scale.x / 2;
    vertices[1] = pParentTransform->position.y + pParentTransform->scale.y / 2;
    vertices[2] = pParentTransform->position.z;
    vertices[3] = pParentTransform->position.x - pParentTransform->scale.x / 2;
    vertices[4] = pParentTransform->position.y + pParentTransform->scale.y / 2;
    vertices[5] = pParentTransform->position.z;
    vertices[6] = pParentTransform->position.x - pParentTransform->scale.x / 2;
    vertices[7] = pParentTransform->position.y - pParentTransform->scale.y / 2;
    vertices[8] = pParentTransform->position.z;
    vertices[9] = pParentTransform->position.x + pParentTransform->scale.x / 2;
    vertices[10] = pParentTransform->position.y - pParentTransform->scale.y / 2;
    vertices[11] = pParentTransform->position.z;

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 0;
    indices[4] = 2;
    indices[5] = 3;
}

void Mesh::normalise_vertices()
{
    const int screen_width = 800;
    const int screen_height = 600;
    for (int i = 0; i < 12; i += 3)
    {
        // not implemented
        continue;
        // z remains the same
    }
}

