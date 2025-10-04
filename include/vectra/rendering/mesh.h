//
// Created by carlos on 9/29/25.
//

#ifndef VECTRA_MESH_H
#define VECTRA_MESH_H

#include <vector>

#include "vectra/rendering/shader.h"
#include "vectra/rendering/texture.h"
#include "vectra/rendering/vertex.h"


class Mesh {
    public:
        // mesh data
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;
        unsigned int VAO;

        Mesh(std::vector<Vertex> c_vertices, std::vector<unsigned int> c_indices, std::vector<Texture> c_textures);
        void draw(Shader &shader);
    private:
        //  render data
        unsigned int VBO, EBO;

        void setup_mesh();
};

#endif //VECTRA_MESH_H