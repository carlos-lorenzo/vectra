#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "gameobject.h"
#include "vectra/rendering/renderer.h"
#include "vectra/rendering/light_source.h"
#include "linkit/linkit.h"



int main()
{
    Renderer scene_renderer(2560, 1440);
    Scene scene;
    GameObject particle_1;
    particle_1.rb.transform.position = linkit::Vector3(5, 0, 0);
    scene.add_game_object(particle_1);

    LightSource scene_light = LightSource(glm::vec3(0, 10, 10), glm::vec3(1.0, 1.0, 1.0));
    scene.add_light_source(scene_light);

    GameObject particle_2;
    particle_2.rb.transform.position = linkit::Vector3(-5, 0, 0);
    scene.add_game_object(particle_2);
    //scene.add_game_object(sun_2);


    scene_renderer.play_scene(scene);

    // linkit::Vector3 vec = linkit::Vector3(0, 0, 0);
    // std::cout << vec.to_string() << std::endl;

    return 0;
}