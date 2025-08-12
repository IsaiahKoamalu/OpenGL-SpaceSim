#ifndef MATERIAL_H
#define MATERIAL_H

#include "Rendering/Shader.h"
#include <glm/glm.hpp>

struct Material
{
    Shader* shader = nullptr;
    glm::vec3 albedo{0.7f, 0.7f, 0.8f};

    // EMissive
    glm::vec3 emissiveColor{0.0f, 0.0f, 0.0f};
    float emissiveIntensity = 6.0f;
    bool useEmissiveTex = false;
    unsigned emissiveTex = 0;
    bool isEmissive = false;

};

#endif
