#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H
#include "Core/Registry.h"
#include "GLFW/glfw3.h"

void inputSystem(GLFWwindow* win, Registry& registry, Entity ship, float dt);

#endif
