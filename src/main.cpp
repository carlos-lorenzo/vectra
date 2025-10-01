#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "gameobject.h"
#include "vectra/rendering/renderer.h"




int main()
{
    Renderer scene_renderer(800, 600);
    Scene scene;
    GameObject obj;
    obj.rb.transform.rotation = linkit::Vector3(20, 0, 30);
    scene.add_game_object(obj);

    scene_renderer.play_scene(scene);

    return 0;
}