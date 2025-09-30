//
// Created by carlos on 9/29/25.
//

#include "vectra/rendering/mesh.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


Mesh::Mesh()
    : some_shader( // This tells the compiler to initialize the 'some_shader' member...
        (std::string(RESOURCES_PATH) + "/shaders/identity.vert").c_str(), // ...by calling the Shader constructor that takes two const char* arguments.
        (std::string(RESOURCES_PATH) + "/shaders/identity.frag").c_str()
      )
{
    // The constructor body now runs AFTER some_shader has been successfully initialized.
    VAO = 0;
    VBO = 0;
    EBO = 0;
    load_vertex_data();
}

void Mesh::load_vertex_data()
{
    float vertices[] = {
        // positions
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f,  // top left
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    // Defining VBO as a general buffer object
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    // Setting VBO to be an array buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Copying vertex data to buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    // specifies how OpenGL should interpret the vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);



}


