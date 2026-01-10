#include <thread>
#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>


#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


#include "vectra/rendering/renderer.h"

#include "camera.h"
#include "utils.h"
#include "vectra/core/scene.h"

void framebuffer_size_callback(GLFWwindow* window, const int width, const int height)
{
    glViewport(0, 0, width, height);
}


static void process_input(GLFWwindow* window, Camera& camera, const double dt)
{
    // Read direct mouse state instead of using external bool
    bool is_captured = glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;

    // Handle losing focus (ESCAPE)
    if (is_captured && glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        if (glfwRawMouseMotionSupported())
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);

        // Let ImGui manage the cursor again
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;

        is_captured = false;
    }

    // Handle gaining focus (Click inside window)
    if (!is_captured && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        // Only capture if we are NOT clicking on an ImGui window
        if (!ImGui::GetIO().WantCaptureMouse)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            if (glfwRawMouseMotionSupported())
                glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

            // Prevent ImGui from showing the cursor or changing its shape
            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

            is_captured = true;
        }
    }

    // State tracking for smooth mouse transition
    static double last_x = 0.0, last_y = 0.0;
    static double yaw = 0.0;
    static double pitch = 0.0;
    static bool first_mouse = true;
    static bool was_captured = false;

    // If not captured, do not process gameplay movement/rotation
    if (!is_captured)
    {
        was_captured = false;
        return;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // WASD movement
    linkit::real speed_multiplier = 1;
    linkit::Vector3 movement_direction;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) movement_direction += linkit::Vector3(0.0f, 0.0f,  1.0f);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) movement_direction += linkit::Vector3(0.0f, 0.0f, -1.0f);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) movement_direction += linkit::Vector3(-1.0f, 0.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) movement_direction += linkit::Vector3( 1.0f, 0.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) movement_direction += linkit::Vector3( 0.0f, 1.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) movement_direction += linkit::Vector3( 0.0f, -1.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) speed_multiplier = 3;

    if (movement_direction * movement_direction > 0.0f) movement_direction.normalize();

    const linkit::Vector3 forward = camera.transform.forward();
    const linkit::Vector3 right = camera.transform.right_dir();
    const linkit::Vector3 up = camera.transform.up_dir();
    const linkit::real speed = speed_multiplier * camera.movement_speed * static_cast<linkit::real>(dt);
    camera.transform.position += speed * (forward * movement_direction.z + right * movement_direction.x + up * movement_direction.y);

    // Mouse look
    double x_pos, y_pos;
    glfwGetCursorPos(window, &x_pos, &y_pos);

    // Reset deltas if we just re-acquired focus to prevent view jumping
    if (first_mouse || !was_captured)
    {
        last_x = x_pos;
        last_y = y_pos;
        first_mouse = false;
    }
    was_captured = true;

    const double dx = x_pos - last_x;
    const double dy = y_pos - last_y;
    last_x = x_pos;
    last_y = y_pos;

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





    // // Setup Dear ImGui context
    // IMGUI_CHECKVERSION();
    // ImGui::CreateContext();
    // ImGuiIO& io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch
    //
    // // Setup Platform/Renderer backends
    // ImGui_ImplGlfw_InitForOpenGL(pWindow_, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    // ImGui_ImplOpenGL3_Init();

    glfwSetInputMode(pWindow_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(pWindow_, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    debug_drawer_ = std::make_unique<DebugDrawer>();
    glfwSetFramebufferSizeCallback(pWindow_, framebuffer_size_callback);
}

void Renderer::begin_frame()
{
    glfwPollEvents();
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Renderer::render_scene_frame(Scene& scene, const glm::mat4& projection_matrix, linkit::real dt)
{
    process_input(pWindow_, scene.camera, dt);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view_matrix = scene.camera.get_view_matrix();


    for (auto obj : scene.game_objects) {
        glm::mat4 model_matrix = Camera::get_model_matrix(obj);
        draw_game_object(obj, model_matrix, view_matrix, projection_matrix, scene);
    }

    // Visualise BVH
    // if (scene.bvh_root) {
    //     debug_drawer_->draw_bvh(scene.bvh_root.get(), view_matrix, projection_matrix);
    // }

    scene.skybox.draw(view_matrix, projection_matrix);
}


void Renderer::end_frame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(pWindow_);
}

void Renderer::draw_frame(Scene &scene, const glm::mat4 &projection_matrix, double dt)
{

    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow(); // Show demo window! :)


    process_input(pWindow_, scene.camera, dt);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view_matrix = scene.camera.get_view_matrix();


    for (auto obj : scene.game_objects) {
        glm::mat4 model_matrix = Camera::get_model_matrix(obj);
        draw_game_object(obj, model_matrix, view_matrix, projection_matrix, scene);
    }

    // Visualise BVH
    // if (scene.bvh_root) {
    //     debug_drawer_->draw_bvh(scene.bvh_root.get(), view_matrix, projection_matrix);
    // }

    scene.skybox.draw(view_matrix, projection_matrix);

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(pWindow_);
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

    // No idea why this is needed
    scene.skybox.skybox_shader.use();
    scene.skybox.skybox_shader.set_int("skybox", 0);



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
    // ImGui_ImplOpenGL3_Shutdown();
    // ImGui_ImplGlfw_Shutdown();
    // ImGui::DestroyContext();

    glfwTerminate();
    glfwDestroyWindow(pWindow_);
    glfwTerminate();
}


void Renderer::draw_game_object(GameObject& obj, glm::mat4 model_matrix, glm::mat4 view_matrix, glm::mat4 projection_matrix, Scene& scene)
{

    obj.shader.use();
    obj.shader.set_mat4("model", model_matrix);
    obj.shader.set_mat4("view", view_matrix);
    obj.shader.set_mat4("projection", projection_matrix);
    glm::vec3 camera_position = vector3_to_vec3(scene.camera.transform.position);

    obj.shader.set_vec3("camera_position", camera_position);

    if (!scene.light_sources.empty())
    {
        obj.shader.set_vec3("light_position", scene.light_sources[0].position);
        obj.shader.set_vec3("light_colour", scene.light_sources[0].colour);
    } else
    {
        glm::vec3 light_position = glm::vec3(0.0f);
        glm::vec3 light_colour = glm::vec3(1.0f);
        obj.shader.set_vec3("light_position", light_position);

        obj.shader.set_vec3("light_position", light_position);
        obj.shader.set_vec3("light_colour", light_colour);
    }
    obj.model.draw(obj.shader);
}
