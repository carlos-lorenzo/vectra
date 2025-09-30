#ifndef VECTRA_RENDERER_H
#define VECTRA_RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../core/scene.h"

class Renderer
{
    private:
        GLFWwindow *pWindow_;
        void draw_frame(Scene &scene) const; //will take a Scene object that does not yet exist
    public:
        Renderer(int width, int height);
        void play_scene(Scene &scene) const; //will take a Scene object that does not yet exist
        static void draw_game_object(const GameObject& obj);
};
#endif //VECTRA_RENDERER_H