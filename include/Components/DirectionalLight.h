#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include <glm/glm.hpp>

struct DirectionalLight
{
    glm::vec3 direction = glm::normalize(glm::vec3(-1, -1, 00.3));
    glm::vec3 color = glm::vec3(1.0f, 0.98f, 0.9f);
    float intensity = 5.0f;
};

#endif
