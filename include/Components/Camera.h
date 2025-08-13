#pragma once
#include <glm/glm.hpp>

struct Camera
{
    glm::vec3 pos{0.0f, 0.0f, 15.0f};
    float yaw = -1.5707963f;
    float pitch = 0.0f;
    float speed = 10.0f;
    float sensitivity = 0.002f;
};