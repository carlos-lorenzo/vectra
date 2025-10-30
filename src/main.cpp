#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "gameobject.h"
#include "vectra/rendering/renderer.h"
#include "vectra/rendering/light_source.h"
#include "vectra/physics/forces/simple_gravity.h"
#include "vectra/physics/forces/newtonian_gravity.h"
#include "linkit/linkit.h"



int main()
{
    Renderer scene_renderer(2560, 1440);
    Scene scene;

    LightSource scene_light = LightSource(glm::vec3(0, 10, 10), glm::vec3(1.0, 1.0, 1.0));
    scene.add_light_source(scene_light);


    GameObject particle_1;
    particle_1.rb.transform.position = linkit::Vector3(5, 0, 0);
    particle_1.rb.velocity = linkit::Vector3(0, 1, 0);
    scene.add_game_object(particle_1);

    GameObject particle_2;
    particle_2.rb.transform.position = linkit::Vector3(-5, 0, 0);
    particle_2.rb.velocity = linkit::Vector3(-1, 1, 0);

    scene.add_game_object(particle_2);

    GameObject particle_3;
    particle_3.rb.transform.position = linkit::Vector3(0, 10, 0);
    particle_3.rb.velocity = linkit::Vector3(1, 0, 0);

    scene.add_game_object(particle_3);


    for (auto &obj : scene.game_objects)
    {
        scene.force_registry.add(&obj, std::make_shared<NewtonianGravity>(50));
    }

    //scene.force_registry.add(&scene.game_objects[1], std::make_shared<NewtonianGravity>(1));

    scene_renderer.play_scene(scene);
    return 0;
}