#ifndef VECTRA_RENDERER_H
#define VECTRA_RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../core/scene.h"

class Renderer
{
    private:
        GLFWwindow *pWindow_;
        float simulation_frequency_ = 60.0f; // Physics update frequency in Hz
        float target_fps_ = 144.0f; // Target frames per second for rendering
        void draw_frame(const Scene &scene);
    public:
        Renderer(int width, int height);
        void play_scene(Scene &scene);
        void cleanup(const Scene& scene);
        static void draw_game_object(GameObject& obj, const Camera& cam);
};
#endif //VECTRA_RENDERER_H