#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "vectra/rendering/shader.h"


void framebuffer_size_callback(GLFWwindow* window, const int width, const int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    std::string vertexShaderPath = std::string(RESOURCES_PATH) + "/shaders/test.vert";
    std::string fragmentShaderPath = std::string(RESOURCES_PATH) + "/shaders/test.frag";

    const Shader shaderProgram(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

    const float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        -0.25f, 0.5f, 0.0f,
        0.0f,  -0.5f, 0.0f,
        0.25f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
    };

    const int indices[] = {
        0, 1, 2,
        2, 3, 4
    };

    unsigned int VBO; //GLuint
    unsigned int VAO;
    unsigned int EBO; // Element Buffer Object

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

    // Render loop
    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        // Rendering commands here
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shaderProgram.use();

        float time = glfwGetTime();
        float greenValue = (std::sin(time) / 2.0f) + 0.5f;
        shaderProgram.set_vec4("current_color", 0.0f, greenValue, 0.0f, 1.0f);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    shaderProgram.delete_program();

    glfwTerminate();
    return 0;
}