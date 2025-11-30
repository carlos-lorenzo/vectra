#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "gameobject.h"
#include "vectra/rendering/renderer.h"
#include "vectra/rendering/light_source.h"
#include "vectra/physics/forces/simple_gravity.h"
#include "vectra/physics/forces/newtonian_gravity.h"
#include "vectra/physics/forces/anchored_spring.h"
#include "linkit/linkit.h"



int main()
{
    Renderer scene_renderer(1920, 1080);
    Scene scene;

    LightSource scene_light = LightSource(glm::vec3(0, 10, -10), glm::vec3(1.0, 1.0, 1.0));
    scene.add_light_source(scene_light);


    GameObject particle_1;

    particle_1.rb.transform.position = linkit::Vector3(0, 0, 0);
    //particle_1.rb.angular_acceleration = linkit::Vector3(1, 1, 1);
    particle_1.rb.velocity = linkit::Vector3(0, 0, 0);
    scene.add_game_object(particle_1);

    GameObject particle_2;
    particle_2.rb.transform.position = linkit::Vector3(-10, 0, 0);
    particle_2.rb.velocity = linkit::Vector3(0, 0, 0);
    scene.add_game_object(particle_2);



    linkit::Vector3 anchor = linkit::Vector3(-1.0, 0.0, 0.0);


    //scene.force_registry.add(&scene.game_objects[0], std::make_shared<AnchoredSpring>(anchor, 1, 1, 1));
    scene.force_registry.add(&scene.game_objects[0], std::make_shared<SimpleGravity>(anchor));
    scene_renderer.play_scene(scene);
    return 0;
}

