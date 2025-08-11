#ifndef VELOCITY_H
#define VELOCITY_H

#include <glm/glm.hpp>

struct Velocity
{
    glm::vec3 linear{0.f}; // in m/s
    glm::vec3 angular{0.f}; // in rad/s, local XYZ = roll/pitch/yaw
};

#endif
