#pragma once
#include <glm\glm.hpp>

struct PointLight
{
    glm::vec3 color{1.0f, 0.95f, 0.85f};
    float intensity = 500.0f;
};