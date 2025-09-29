//
// Created by carlos on 9/29/25.
//

#ifndef VECTRA_MESH_H
#define VECTRA_MESH_H

#include "vectra/physics/transform.h"

class Mesh
{
    public:
        Transform *pParentTransform;
        float vertices[12]{};
        unsigned int indices[6]{};

        Mesh();
        explicit Mesh(Transform *parentTransform);
        // Child class will implement its own method in the future, for now set square as default
        void construct_vertices();
        void normalise_vertices();

};


#endif //VECTRA_MESH_H