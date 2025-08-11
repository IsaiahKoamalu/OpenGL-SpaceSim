#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct Transform
{
    glm::vec3 position{0.f};
    glm::quat rotation{1,0,0,0};
    glm::vec3 scale{1.f,1.f,1.f};
};

#endif
