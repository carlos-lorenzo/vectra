#include <thread>
#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>

#include "vectra/rendering/renderer.h"
#include "vectra/core/scene.h"

void framebuffer_size_callback(GLFWwindow* window, const int width, const int height)
{
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *window, Camera& camera)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    linkit::Vector3 movement_direction;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        movement_direction += linkit::Vector3(0.0f, 0.0f, 1.0f);

    } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        movement_direction -= linkit::Vector3(0.0f, 0.0f, 1.0f);

    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        movement_direction += linkit::Vector3(-1.0f, 0.0f, 0.0f);
    } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        movement_direction -= linkit::Vector3(-1.0f, 0.0f, 0.0f);
    }
    movement_direction.normalize();


    camera.transform.position += camera.movement_speed * camera.transform.forward() * movement_direction.z;
    camera.transform.position += camera.movement_speed * camera.right_direction.axis() * movement_direction.x;
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
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST); // Enable depth testing
    glViewport(0, 0, width, height);

}

void Renderer::draw_frame(Scene &scene, const glm::mat4 &projection_matrix)
{

    process_input(pWindow_, scene.camera);
    // will be replaced by scene background/skybox...
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::mat4 view_matrix = scene.camera.get_view_matrix();
    for (auto obj : scene.game_objects)
    {
        glm::mat4 model_matrix = Camera::get_model_matrix(obj);
        draw_game_object(obj, model_matrix, view_matrix, projection_matrix);
    }

    glfwSwapBuffers(pWindow_);
    glfwPollEvents();
}


void Renderer::play_scene(Scene &scene)
{

    glfwSetFramebufferSizeCallback(pWindow_, framebuffer_size_callback);
    double t = 0.0;
    const double dt = 1.0 / simulation_frequency_; // Physics timestep (60 Hz)

    double currentTime = glfwGetTime();
    double accumulator = 0.0;

    const double min_frame_time = 1.0 / target_fps_;



    glm::mat4 projection_matrix = scene.camera.get_projection_matrix();


    while(!glfwWindowShouldClose(pWindow_))
    {
        double new_time = glfwGetTime();
        double frame_time = new_time - currentTime;
        currentTime = new_time;

        // display FPS in window title
        static double fps_timer = 0.0;
        static int fps_frames = 0;
        fps_timer += frame_time;
        fps_frames++;
        if (fps_timer >= 1.0) {
            std::string title = "Vectra - FPS: " + std::to_string(fps_frames);
            glfwSetWindowTitle(pWindow_, title.c_str());
            fps_timer -= 1.0;
            fps_frames = 0;
        }
        accumulator += frame_time;

        while (accumulator >= dt)
        {
            scene.step(static_cast<linkit::real>(dt)); // Update physics with fixed timestep
            accumulator -= dt;
            t += dt;
        }

        draw_frame(scene, projection_matrix);

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
        obj.shader.delete_program();
    }
    glfwTerminate();
    glfwDestroyWindow(pWindow_);
    glfwTerminate();
}


void Renderer::draw_game_object(GameObject& obj, glm::mat4 model_matrix, glm::mat4 view_matrix, glm::mat4 projection_matrix)
{

    obj.shader.use();
    obj.shader.set_mat4("model", model_matrix);
    obj.shader.set_mat4("view", view_matrix);
    obj.shader.set_mat4("projection", projection_matrix);
    obj.model.draw(obj.shader);
}
