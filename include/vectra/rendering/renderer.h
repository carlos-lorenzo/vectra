#ifndef VECTRA_RENDERER_H
#define VECTRA_RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "vectra/rendering/debug_drawer.h"
#include "vectra/core/scene.h"

class Renderer
{
    private:
        GLFWwindow *pWindow_;
        float simulation_frequency_ = 60.0f; // Physics update frequency in Hz
        float target_fps_ = 144.0f; // Target frames per second for rendering

        std::unique_ptr<DebugDrawer> debug_drawer_; // Add debug drawer
        void draw_frame(Scene &scene, const glm::mat4 &projection_matrix, double dt);
    public:
        Renderer(int width, int height);
        [[nodiscard]] GLFWwindow* get_window() const { return pWindow_; }
        void begin_frame();
        void render_scene_frame(Scene &scene, const glm::mat4 &projection_matrix, linkit::real dt);
        void end_frame();
        void play_scene(Scene &scene);
        void cleanup(const Scene& scene);
        static void draw_game_object(::GameObject& obj, glm::mat4 model_matrix, glm::mat4 view_matrix, glm::mat4 projection_matrix, Scene& scene);
};
#endif //VECTRA_RENDERER_H