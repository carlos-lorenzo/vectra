#ifndef VECTRA_RENDERER_H
#define VECTRA_RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "vectra/rendering/debug_drawer.h"
#include "vectra/rendering/shader.h"
#include "vectra/rendering/model.h"
#include "vectra/rendering/framebuffer.h"
#include "vectra/rendering/scene_lights.h"

#include "vectra/core/engine_state.h"
#include "vectra/core/scene.h"
#include "vectra/core/scene_snapshot.h"

class Renderer
{
    private:
        GLFWwindow *pWindow_;
        EngineState* state_;
        std::unique_ptr<DebugDrawer> debug_drawer_; // Add debug drawer
        std::unordered_map<std::string, Model> model_cache_; // Model cache
        std::unique_ptr<Shader> model_shader_; // Add shader
        std::unique_ptr<Shader> depth_shader_; // NEW: depth-only shader for shadow pass
        Camera camera_; // Camera for rendering (set by scene)
        SceneLights scene_lights_; // Scene lights for rendering (set by scene)
        std::unique_ptr<Skybox> skybox_; // Skybox for rendering (set by scene) - must be initialized after OpenGL context
        glm::mat4 projection_matrix_{};
        std::unique_ptr<Framebuffer> scene_fbo_; // Framebuffer for scene rendering




    public:
        Renderer(EngineState* state);
        void initialize_window();
        void setup_from_scene(const Scene& scene);
        [[nodiscard]] GLFWwindow* get_window() const { return pWindow_; }
        void use_skybox();
        static void begin_frame();
        void render_scene_frame(Scene &scene, linkit::real dt);
        void render_snapshot_frame(Skybox &skybox, const SceneSnapshot &snapshot, linkit::real dt);
        void render_to_framebuffer(const SceneSnapshot &snapshot, linkit::real dt);
        void resize_framebuffer(int width, int height);
        [[nodiscard]] GLuint get_scene_texture_id() const;
        void end_frame() const;

        void cleanup(const Scene& scene);
    private:
        void draw_game_object(::GameObject& obj, glm::mat4 model_matrix, glm::mat4 view_matrix, glm::mat4 projection_matrix, Scene& scene);
        void draw_game_object(const std::string& model_name, glm::mat4 model_matrix, glm::mat4 view_matrix, glm::mat4 projection_matrix, glm::vec3 camera_position);

        // Shadow mapping
        // Render shadow maps for lights that can cast shadows. 'dt' passed by value (no const qualifier).
        void render_shadow_maps(const SceneSnapshot& snapshot, linkit::real dt); // NEW: stub for shadow pass

};
#endif //VECTRA_RENDERER_H