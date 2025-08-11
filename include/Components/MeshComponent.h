#ifndef MESHCOMPONENT_H
#define MESHCOMPONENT_H

#include "Rendering/Mesh.h"

struct MeshComponent
{
    Mesh mesh; // this owns VBO/VAO/EBO
};

#endif