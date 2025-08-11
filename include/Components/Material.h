#ifndef MATERIAL_H
#define MATERIAL_H

#include "Rendering/Shader.h"
#include <glm/glm.hpp>

struct Material
{
    Shader* shader = nullptr;
    glm::vec3 albedo{0.7f, 0.7f, 0.8f};
};

#endif
