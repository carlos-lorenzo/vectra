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


static void process_input(GLFWwindow* window, Camera& camera, const double dt)
{
    linkit::real speed_multiplier = 1;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // WASD movement
    linkit::Vector3 movement_direction;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) movement_direction += linkit::Vector3(0.0f, 0.0f,  1.0f);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) movement_direction += linkit::Vector3(0.0f, 0.0f, -1.0f);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) movement_direction += linkit::Vector3(-1.0f, 0.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) movement_direction += linkit::Vector3( 1.0f, 0.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) speed_multiplier = 3;

    if (movement_direction * movement_direction > 0.0f) movement_direction.normalize();

    const linkit::Vector3 forward = camera.transform.forward();
    const linkit::Vector3 right = camera.transform.right_dir();
    const linkit::real speed = speed_multiplier * camera.movement_speed * static_cast<linkit::real>(dt);
    camera.transform.position += speed * (forward * movement_direction.z + right * movement_direction.x);

    // Mouse look
    static bool first = true;
    static double lastX = 0.0, lastY = 0.0;
    static double yaw = 0.0;
    static double pitch = 0.0;

    double x, y;
    glfwGetCursorPos(window, &x, &y);
    if (first) { lastX = x; lastY = y; first = false; }

    const double dx = x - lastX;
    const double dy = y - lastY;
    lastX = x; lastY = y;

    yaw += dx * camera.mouse_sensitivity;
    pitch -= dy * camera.mouse_sensitivity;

    pitch = std::clamp(pitch, -camera.max_pitch, camera.max_pitch);

    // Reconstruct rotation from yaw and pitch
    const linkit::Vector3 world_up(0.0f, 1.0f, 0.0f);
    const linkit::Vector3 world_right(1.0f, 0.0f, 0.0f);

    const linkit::Quaternion q_yaw(static_cast<linkit::real>(-yaw), world_up);
    const linkit::Quaternion q_pitch(static_cast<linkit::real>(pitch), world_right);

    camera.transform.rotation = q_yaw * q_pitch;
    camera.transform.rotation.normalize();
}


Renderer::Renderer(const int width, const int height)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    pWindow_ = glfwCreateWindow(width, height, "Vectra", nullptr, nullptr);
    if (pWindow_ == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(pWindow_);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return;
    }

    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glViewport(0, 0, width, height);

    // Lock/hide cursor and use raw mouse motion if available
    glfwSetInputMode(pWindow_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(pWindow_, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
}

void Renderer::draw_frame(Scene &scene, const glm::mat4 &projection_matrix, double dt)
{
    process_input(pWindow_, scene.camera, dt);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view_matrix = scene.camera.get_view_matrix();
    for (auto obj : scene.game_objects) {
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
    const double dt_phys = 1.0 / simulation_frequency_;

    double currentTime = glfwGetTime();
    double accumulator = 0.0;

    const double min_frame_time = 1.0 / target_fps_;
    glm::mat4 projection_matrix = scene.camera.get_projection_matrix();

    while (!glfwWindowShouldClose(pWindow_)) {
        double new_time = glfwGetTime();
        double frame_time = new_time - currentTime;
        currentTime = new_time;

        // FPS counter
        static double fps_timer = 0.0; static int fps_frames = 0;
        fps_timer += frame_time; fps_frames++;
        if (fps_timer >= 1.0) {
            std::string title = "Vectra - FPS: " + std::to_string(fps_frames);
            glfwSetWindowTitle(pWindow_, title.c_str());
            fps_timer -= 1.0; fps_frames = 0;
        }

        accumulator += frame_time;
        while (accumulator >= dt_phys) {
            scene.step(static_cast<linkit::real>(dt_phys));
            accumulator -= dt_phys;
            t += dt_phys;
        }

        draw_frame(scene, projection_matrix, frame_time);

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
