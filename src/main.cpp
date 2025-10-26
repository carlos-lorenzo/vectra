#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "gameobject.h"
#include "vectra/rendering/renderer.h"
#include "linkit/linkit.h"



int main()
{
    Renderer scene_renderer(2560/2, 1440);
    Scene scene;
    GameObject sun;


    scene.add_game_object(sun);

    scene_renderer.play_scene(scene);

    // linkit::Vector3 vec = linkit::Vector3(0, 0, 0);
    // std::cout << vec.to_string() << std::endl;

    return 0;
}