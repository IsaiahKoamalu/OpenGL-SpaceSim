#pragma once
#include <glm/glm.hpp>

struct Transform
{
    glm::vec3 position{0.0f};
    glm::vec3 rotationEuler{0.0f};
    glm::vec3 scale{1.0f};
};