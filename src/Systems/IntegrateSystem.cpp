#include "Systems/IntegrateSystem.h"
#include "Components/Transform.h"
#include "Components/Velocity.h"
#include <glm/gtc/quaternion.hpp>

void integrateSystem(Registry& reg, float dt)
{
    reg.for_each<Transform, Velocity>([&](Entity, Transform& t, Velocity& v)
    {
       t.position += v.linear * dt;

        // integration of orientation by angular velocity
        glm::quat dq = glm::quat(0, v.angular.x, v.angular.y, v.angular.z) * t.rotation;
        t.rotation = glm::normalize(t.rotation + 0.5f * dq * dt);
    });
}