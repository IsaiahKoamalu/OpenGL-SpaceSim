#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <glm/gtc/matrix_transform.hpp>
#include "Core/Registry.h"
#include "Rendering/Shader.h"
#include "Components/Camera.h"

struct CameraMatrices
{
    glm::mat4 view, proj;
    glm::vec3 eyeWorld;
};

CameraMatrices computeCamera(const Registry& reg, Entity cameraEntity);
void renderSystem(Registry& reg, Shader& shader, const CameraMatrices& cam);

#endif
