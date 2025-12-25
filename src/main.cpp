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
    // particle_1.set_collider_type("ColliderBox");
    // particle_1.set_shape("cube");
    particle_1.set_collider_type("ColliderSphere");
    particle_1.set_shape("sphere");
    particle_1.rb.transform.position = linkit::Vector3(3, 0, 0);
    //particle_1.rb.angular_acceleration = linkit::Vector3(1, 1, 1);
    particle_1.rb.velocity = linkit::Vector3(0, 0, 0);
    scene.add_game_object(particle_1);

    GameObject particle_2;
    particle_2.set_collider_type("ColliderBox");
    particle_2.set_shape("cube");
    // particle_2.set_collider_type("ColliderSphere");
    // particle_2.set_shape("sphere");
    particle_2.rb.transform.position = linkit::Vector3(0, 0, 0);
    particle_2.rb.velocity = linkit::Vector3(0, 0, 0);
    //particle_2.rb.transform.scale = linkit::Vector3(1, 0.5, 0.5);
    scene.add_game_object(particle_2);


    GameObject floor;
    floor.set_collider_type("ColliderBox");
    floor.set_shape("cube");
    floor.rb.mass = 1000;
    floor.rb.inverse_mass = 0.001;

    // particle_2.set_collider_type("ColliderSphere");
    // particle_2.set_shape("sphere");
    floor.rb.transform.position = linkit::Vector3(0, -5, 0);
    floor.rb.transform.scale = linkit::Vector3(10, 1, 10);
    scene.add_game_object(floor);



    linkit::Vector3 anchor = linkit::Vector3(3, 1, 0.0);
    linkit::Vector3 gravity = linkit::Vector3(0, -3, 0.0);
    linkit::Vector3 wind = linkit::Vector3(-1, 0, 0.0);
    //scene.force_registry.add(&scene.game_objects[0], std::make_shared<AnchoredSpring>(anchor, 1, 1, 1));
    scene.force_registry.add(&scene.game_objects[0], std::make_shared<SimpleGravity>(wind));
    scene.force_registry.add(&scene.game_objects[1], std::make_shared<SimpleGravity>(-3*wind));
    scene_renderer.play_scene(scene);
    return 0;
}

