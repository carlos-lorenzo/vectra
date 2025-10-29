#include <glm/glm.hpp>

#include "vectra/rendering/light_source.h"


LightSource::LightSource()
{
    position = glm::vec3(0.0);
    colour = glm::vec3(1.0);
}
LightSource::LightSource(glm::vec3 light_position, glm::vec3 light_colour)
{
    position = light_position;
    colour = light_colour;
}

