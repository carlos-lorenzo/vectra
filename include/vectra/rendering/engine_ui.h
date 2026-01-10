#ifndef VECTRA_ENGINE_UI_H
#define VECTRA_ENGINE_UI_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "vectra/core/engine_state.h"
#include "vectra/core/scene.h"

class EngineUI
{
public:
    static void initialize(GLFWwindow* window);
    void draw(EngineState& state, Scene& scene);
    void cleanup();
};
#endif //VECTRA_ENGINE_UI_H
