//
// Created by carlos on 9/29/25.
//

#ifndef VECTRA_MESH_H
#define VECTRA_MESH_H

#include "vectra/rendering/shader.h"


class Mesh
{
    public:
        unsigned int VBO;
        unsigned int VAO;
        unsigned int EBO;

        // Temporary shader for testing purposes will be split into material and transform shader?
        Shader some_shader;

        Mesh();
        void load_vertex_data();

};


#endif //VECTRA_MESH_H