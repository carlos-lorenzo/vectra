#ifndef VECTRA_LIGHT_SOURCE_H
#define VECTRA_LIGHT_SOURCE_H
#include <glm/vec3.hpp>

// Maybe use struct instead of class? Not sure if I want to add more complicated behaviours, i.e. spotlights

class LightSource
{
    public:
        glm::vec3 position;
        glm::vec3 colour;

    LightSource();
    LightSource(glm::vec3 light_position, glm::vec3 light_colour);
};

#endif //VECTRA_LIGHT_SOURCE_H