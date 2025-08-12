#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include <glm/glm.hpp>

struct PointLight
{
    glm::vec3 position;
    glm::vec3 color = glm::vec3(1.0f, 0.98f, 0.9f);
    float intensity = 50.0f;
    float radius = 100.0f;
};

#endif
