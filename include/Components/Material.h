#ifndef MATERIAL_H
#define MATERIAL_H

#include "Rendering/Shader.h"
#include <glm/glm.hpp>

struct Material
{
    Shader* shader = nullptr;
    glm::vec3 albedo{0.7f, 0.7f, 0.8f};
    glm::vec3 emissiveColor{0.0f, 0.0f, 0.0f};
    float emissiveIntensity = 1.0f;
    bool useEmissiveTex = false;

};

#endif
