#include "Systems/CameraFollowSystem.h"
#include "Components/Transform.h"
#include "Components/Camera.h"
#include <glm/gtc/quaternion.hpp>

void cameraFollowSystem(Registry& reg, Entity cameraE, Entity shipE)
{
    auto& ct = reg.get<Transform>(cameraE);
    auto& st = reg.get<Transform>(shipE);

    glm::vec3 offset(0.f, 0.5f, 0.1f);
    ct.rotation = st.rotation;
    ct.position = st.position + glm::mat3_cast(st.rotation) * offset;
}
