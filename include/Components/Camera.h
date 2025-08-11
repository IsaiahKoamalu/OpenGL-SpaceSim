#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

struct Camera
{
    float fov = 60.f;
    float nearPlane = 0.1f;
    float farPlane = 1000.f;
    bool primary = true;
};

#endif
