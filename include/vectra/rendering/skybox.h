#ifndef VECTRA_SKYBOX_H
#define VECTRA_SKYBOX_H

#include <vector>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "vectra/rendering/shader.h"

class Skybox
{
    // Six paths
    std::vector<std::string> faces;


    public:
    Shader skybox_shader;
    unsigned int skybox_vao;
    unsigned int skybox_vbo;
    unsigned int cubemap;
    Skybox();
    unsigned int load_cubemap();
    void draw(const glm::mat4& view, const glm::mat4& projection);
};

#endif //VECTRA_SKYBOX_H