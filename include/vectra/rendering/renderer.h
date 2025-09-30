#ifndef VECTRA_RENDERER_H
#define VECTRA_RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../core/scene.h"

class Renderer
{
    private:
        GLFWwindow *pWindow_;
        void draw_frame(const Scene &scene) const; //will take a Scene object that does not yet exist
    public:
        Renderer(int width, int height);
        void play_scene(const Scene &scene) const; //will take a Scene object that does not yet exist
        void cleanup(const Scene& scene) const;
        void cleanup(const Scene& scene);
        static void draw_game_object(const GameObject& obj, const Camera& cam);
};
#endif //VECTRA_RENDERER_H