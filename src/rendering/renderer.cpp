#include <thread>
#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "vectra/rendering/renderer.h"
#include "vectra/core/scene.h"

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
    glEnable(GL_DEPTH_TEST); // Enable depth testing
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(pWindow_, framebuffer_size_callback);
}

void Renderer::draw_frame(const Scene &scene)
{
    process_input(pWindow_);
    // will be replaced by scene background/skybox...
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (int i = 0; i < scene.game_objects.size(); ++i)
    {
        std::vector<GameObject>::value_type obj = scene.game_objects[i];
        draw_game_object(obj, scene.camera);
    }

    glfwSwapBuffers(pWindow_);
    glfwPollEvents();
}


void Renderer::play_scene(Scene &scene)
{
    double t = 0.0;
    const double dt = 1.0 / simulation_frequency_; // Physics timestep (60 Hz)

    double currentTime = glfwGetTime();
    double accumulator = 0.0;


    const double min_frame_time = 1.0 / target_fps_;

    while(!glfwWindowShouldClose(pWindow_))
    {
        double new_time = glfwGetTime();
        double frame_time = new_time - currentTime;
        currentTime = new_time;

        accumulator += frame_time;

        while (accumulator >= dt)
        {
            scene.step(static_cast<linkit::real>(dt)); // Update physics with fixed timestep
            accumulator -= dt;
            t += dt;
        }

        draw_frame(scene);

        // Limit the frame rate
        double end_frame_time = glfwGetTime();
        double time_to_wait = min_frame_time - (end_frame_time - new_time);
        if (time_to_wait > 0.0) {
            std::this_thread::sleep_for(std::chrono::duration<double>(time_to_wait));
        }
    }
    cleanup(scene);
}


void Renderer::cleanup(const Scene &scene)
{
    for (const auto& obj : scene.game_objects)
    {
        // obj.mesh.some_shader.delete_program();
        // glDeleteVertexArrays(1, &obj.mesh.VAO);
        // glDeleteBuffers(1, &obj.mesh.VBO);
        // glDeleteBuffers(1, &obj.mesh.EBO);
    }
    glfwTerminate();
    glfwDestroyWindow(pWindow_);
    glfwTerminate();
}


void Renderer::draw_game_object(GameObject &obj, const Camera& cam)
{

    obj.shader.use();
    glm::mat4 model = Camera::get_model_matrix(obj);
    glm::mat4 view = cam.get_view_matrix();
    glm::mat4 projection = cam.get_projection_matrix();
    obj.shader.set_mat4("model", model);
    obj.shader.set_mat4("view", view);
    obj.shader.set_mat4("projection", projection);
    obj.model.draw(obj.shader);
}
