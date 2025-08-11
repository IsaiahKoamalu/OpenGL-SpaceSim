#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <GLFW/glfw3.h>
#include "../include/System.h"
#include "../include/Engine/UpdateContext.h"
#include "Mesh.h"

class RenderSystem : public System
{
public:
    void update(const UpdateContext& ctxt)
    {
        GLFWwindow& window = *ctxt.window;
    }
};

#endif
