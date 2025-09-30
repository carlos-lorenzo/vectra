#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "vectra/rendering/renderer.h"
#include "../../include/vectra/core/scene.h"

void framebuffer_size_callback(GLFWwindow* window, const int width, const int height)
{
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


Renderer::Renderer(const int width, const int height)
{
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    pWindow_ = glfwCreateWindow(width, height, "Vectra", nullptr, nullptr);
    if (pWindow_ == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(pWindow_);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return;
    }
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(pWindow_, framebuffer_size_callback);
}

void Renderer::draw_frame(const Scene &scene) const
{
    process_input(pWindow_);
    // will be replaced by scene background/skybox...
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    for (const auto& obj : scene.game_objects)
    {
        draw_game_object(obj, scene.camera);
    }

    glfwSwapBuffers(pWindow_);
    glfwPollEvents();
}


void Renderer::play_scene(const Scene &scene) const
{
    while(!glfwWindowShouldClose(pWindow_))
    {
        draw_frame(scene);
    }
    cleanup(scene);
}

void Renderer::cleanup(const Scene &scene) const
{
    for (const auto& obj : scene.game_objects)
    {
        obj.mesh.some_shader.delete_program();
        glDeleteVertexArrays(1, &obj.mesh.VAO);
        glDeleteBuffers(1, &obj.mesh.VBO);
        glDeleteBuffers(1, &obj.mesh.EBO);
    }
    glfwTerminate();
    glfwDestroyWindow(pWindow_);
    glfwTerminate();
}


void Renderer::draw_game_object(const GameObject &obj, const Camera& cam)
{
    obj.mesh.some_shader.use();
    glm::mat4 model = Camera::get_model_matrix(obj);
    glm::mat4 view = cam.get_view_matrix();
    glm::mat4 projection = glm::mat4(1.0f); //cam.get_projection_matrix();
    obj.mesh.some_shader.set_mat4("model", model);
    obj.mesh.some_shader.set_mat4("view", view);
    obj.mesh.some_shader.set_mat4("projection", projection);
    glBindVertexArray(obj.mesh.VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
