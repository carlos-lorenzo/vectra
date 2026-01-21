#include <thread>
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
#include "vectra/rendering/framebuffer.h"



#include "camera.h"
#include "utils.h"
#include "vectra/core/scene.h"

void framebuffer_size_callback(GLFWwindow* window, const int width, const int height)
{
    glViewport(0, 0, width, height);
}


static void process_input(GLFWwindow* window, Camera& camera, const double dt, bool scene_view_focused)
{
    // Read direct mouse state instead of using external bool
    bool is_captured = glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;

    // Handle losing focus (ESCAPE)
    if (is_captured && glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        if (glfwRawMouseMotionSupported())
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);

        // Re-enable ImGui mouse input
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
        io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;

        is_captured = false;
    }

    // Handle gaining focus (Click inside Scene View window)
    // When scene_view_focused is true, we're hovering and focused on the Scene View,
    // so we should capture mouse even though WantCaptureMouse is true (it's our window)
    if (!is_captured && scene_view_focused && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (glfwRawMouseMotionSupported())
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

        // Disable ImGui mouse input entirely while camera is captured
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        io.ConfigFlags |= ImGuiConfigFlags_NoMouse;

        is_captured = true;
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

Renderer::Renderer(EngineState* state)
    : pWindow_(nullptr), state_(state)
{
    initialize_window();


    // Initialize OpenGL-dependent objects AFTER OpenGL context is created
    skybox_ = std::make_unique<Skybox>();
    camera_ = Camera();
    projection_matrix_ = camera_.get_projection_matrix();

    // Create a framebuffer for scene rendering
    scene_fbo_ = std::make_unique<Framebuffer>(state_->scene_view_width, state_->scene_view_height);

    model_shader_ = std::make_unique<Shader>("resources/shaders/model.vert", "resources/shaders/blinn_phong.frag");
    // Load depth-only shader for shadow mapping (stub)
    depth_shader_ = std::make_unique<Shader>("resources/shaders/shadow_mapping_depth.vert", "resources/shaders/shadow_mapping_depth.frag");

    model_cache_.emplace("cube", Model("resources/models/primitives/cube.obj", false));
    model_cache_.emplace("sphere", Model("resources/models/primitives/sphere.obj", false));
    model_cache_.emplace("vector", Model("resources/models/debugging/vector.obj", false));

    scene_lights_ = SceneLights();

}

void Renderer::initialize_window()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    pWindow_ = glfwCreateWindow(state_->window_width, state_->window_height, "Vectra", nullptr, nullptr);
    if (pWindow_ == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(pWindow_);
    glfwSwapInterval(state_->enable_vsync ? 1 : 0);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return;
    }

    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glViewport(0, 0, state_->window_width, state_->window_height);

    // Start with a normal cursor (not captured) - user clicks scene view to capture
    glfwSetInputMode(pWindow_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    debug_drawer_ = std::make_unique<DebugDrawer>();
    glfwSetFramebufferSizeCallback(pWindow_, framebuffer_size_callback);
}

void Renderer::setup_from_scene(const Scene& scene)
{
    camera_ = scene.camera;

    scene_lights_ = scene.scene_lights;
    skybox_ = std::make_unique<Skybox>(scene.skybox);
    projection_matrix_ = camera_.get_projection_matrix();

}

void Renderer::use_skybox()
{
    skybox_->skybox_shader.use();
    skybox_->skybox_shader.set_int("skybox", 0);
}

void Renderer::begin_frame()
{
    glfwPollEvents();

}

void Renderer::render_scene_frame(Scene& scene, linkit::real dt)
{

    process_input(pWindow_, scene.camera, dt, state_->scene_view_focused);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view_matrix = scene.camera.get_view_matrix();


    for (auto obj : scene.game_objects) {
        glm::mat4 model_matrix = Camera::get_model_matrix(obj.rb.transform);
        draw_game_object(obj, model_matrix, view_matrix, projection_matrix_, scene);
    }


    skybox_->draw(view_matrix, projection_matrix_);
}

void Renderer::render_snapshot_frame(Skybox &skybox, const SceneSnapshot& snapshot, const linkit::real dt)
{
    process_input(pWindow_, camera_, dt, state_->scene_view_focused);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view_matrix = camera_.get_view_matrix();
    glm::vec3 camera_position = vector3_to_vec3(camera_.transform.position);

    for (const auto& obj_snapshot : snapshot.object_snapshots) {
        glm::mat4 model_matrix = Camera::get_model_matrix(obj_snapshot.transform);

        draw_game_object(obj_snapshot.model_name, model_matrix, view_matrix, projection_matrix_, camera_position);
    }

    skybox_->draw(view_matrix, projection_matrix_);
}

void Renderer::render_to_framebuffer(const SceneSnapshot& snapshot, const linkit::real dt)
{
    process_input(pWindow_, camera_, dt, state_->scene_view_focused);

    // Resize the framebuffer if needed
    resize_framebuffer(state_->scene_view_width, state_->scene_view_height);

    // Update projection matrix for the new aspect ratio
    float aspect = static_cast<float>(state_->scene_view_width) / static_cast<float>(state_->scene_view_height);
    projection_matrix_ = glm::perspective(
        glm::radians(static_cast<float>(camera_.fov)),
        aspect,
        static_cast<float>(camera_.nearPlane),
        static_cast<float>(camera_.farPlane)
    );

    // NEW: render shadow maps (currently a stub)
    render_shadow_maps(snapshot, dt);

    // Bind framebuffer and render scene
    scene_fbo_->bind();

    glClearColor(0.18f, 0.18f, 0.22f, 1.0f);  // Dracula background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view_matrix = camera_.get_view_matrix();
    glm::vec3 camera_position = vector3_to_vec3(camera_.transform.position);

    for (const auto& obj_snapshot : snapshot.object_snapshots) {
        glm::mat4 model_matrix = Camera::get_model_matrix(obj_snapshot.transform);
        draw_game_object(obj_snapshot.model_name, model_matrix, view_matrix, projection_matrix_, camera_position);

        if (state_->draw_forces)
        {
            debug_drawer_->set_light_sources(scene_lights_, camera_position);
            debug_drawer_->draw_force(obj_snapshot.transform, obj_snapshot.force, view_matrix, projection_matrix_, camera_position);

        }
        if (obj_snapshot.has_spring)
        {
            debug_drawer_->set_light_sources(scene_lights_, camera_position);
            debug_drawer_->draw_spring(obj_snapshot.transform.position, obj_snapshot.spring_anchor, view_matrix, projection_matrix_, camera_position);
        }
        if (state_->draw_bvh && snapshot.bvh_root)
        {
            debug_drawer_->set_light_sources(scene_lights_, camera_position);
            debug_drawer_->draw_bvh(snapshot.bvh_root, view_matrix, projection_matrix_);
        }
    }

    skybox_->draw(view_matrix, projection_matrix_);

    // Unbind framebuffer - return to default
    Framebuffer::unbind();

    // Restore viewport to window size
    int width, height;
    glfwGetFramebufferSize(pWindow_, &width, &height);
    glViewport(0, 0, width, height);
}

void Renderer::resize_framebuffer(int width, int height)
{
    if (scene_fbo_ && width > 0 && height > 0)
    {
        scene_fbo_->resize(width, height);
    }
}

GLuint Renderer::get_scene_texture_id() const
{
    return scene_fbo_ ? scene_fbo_->get_texture_id() : 0;
}


void Renderer::end_frame() const
{
    glfwSwapBuffers(pWindow_);
}



void Renderer::cleanup(const Scene &scene)
{
    model_shader_->delete_program();


    glfwTerminate();
    glfwDestroyWindow(pWindow_);
    glfwTerminate();
}


void Renderer::draw_game_object(GameObject& obj, glm::mat4 model_matrix, glm::mat4 view_matrix, glm::mat4 projection_matrix, Scene& scene)
{
    glm::vec3 camera_position = vector3_to_vec3(scene.camera.transform.position);

    model_shader_->use();
    model_shader_->set_mat4("model", model_matrix);
    model_shader_->set_mat4("view", view_matrix);
    model_shader_->set_mat4("projection", projection_matrix);
    model_shader_->set_vec3("camera_position", camera_position);



    scene_lights_.setup_lighting(*model_shader_, camera_position);

    model_cache_.at(obj.model_name).draw(*model_shader_);
}

void Renderer::draw_game_object(const std::string& model_name, glm::mat4 model_matrix, glm::mat4 view_matrix, glm::mat4 projection_matrix, glm::vec3 camera_position)
{
    model_shader_->use();
    model_shader_->set_mat4("model", model_matrix);
    model_shader_->set_mat4("view", view_matrix);
    model_shader_->set_mat4("projection", projection_matrix);
    model_shader_->set_vec3("camera_position", camera_position);



    scene_lights_.setup_lighting(*model_shader_, camera_position);




    model_cache_.at(model_name).draw(*model_shader_);
}

// Minimal stub implementation for shadow map generation. Will be expanded in follow-up changes.
void Renderer::render_shadow_maps(const SceneSnapshot& snapshot, linkit::real dt)
{
    // Respect global toggle in EngineState
    if (!state_ || !state_->draw_shadows) return;
    if (!depth_shader_) return;

    // For now, just validate that the shader can be used without performing GL FBO work.
    depth_shader_->use();
    // We intentionally do not change any GL state or bind FBOs yet; this is a lightweight placeholder.
}
