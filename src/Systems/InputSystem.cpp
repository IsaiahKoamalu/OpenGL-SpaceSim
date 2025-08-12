#include "Systems/InputSystem.h"
#include "Components/ShipControl.h"
#include "Components/Velocity.h"
#include "Components/Transform.h"
#include "glm/gtc/quaternion.hpp"

static bool firstMouse = true;
static double lastX=0.0, lastY=0.0;

void inputSystem(GLFWwindow* win, Registry& reg, Entity ship, float dt)
{
    auto& sc = reg.get<ShipControl>(ship);
    auto& tr = reg.get<Transform>(ship);
    auto& ve = reg.get<Velocity>(ship);

    // --- mouse look (local yaw/pitch) ---
    double mx, my; glfwGetCursorPos(win, &mx, &my);
    if (firstMouse) { lastX = mx; lastY = my; firstMouse = false; }
    double dx = mx - lastX;
    double dy = lastY - my; // invert Y so up is positive
    lastX = mx; lastY = my;

    float sensRad = sc.mouseSense * 0.017453292519943295f; // deg->rad
    float yaw  = -float(dx) * sensRad;
    float pitch = float(dy) * sensRad;

    const float kMaxPitch = glm::radians(89.0f);
    sc.pitch = glm::clamp(sc.pitch, -kMaxPitch, kMaxPitch);



    // rotate in local frame: yaw around local up, pitch around local right
    glm::vec3 upLocal   = glm::mat3_cast(tr.rotation) * glm::vec3(0,1,0);
    glm::vec3 rightLocal= glm::mat3_cast(tr.rotation) * glm::vec3(1,0,0);
    tr.rotation = glm::normalize(glm::angleAxis(yaw, upLocal) * tr.rotation);
    tr.rotation = glm::normalize(glm::angleAxis(pitch, rightLocal) * tr.rotation);

    // --- keyboard thrust ---
    float boost = (glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? 2.0f : 1.0f;
    glm::vec3 accLocal(0.0f);
    if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) accLocal.z -= sc.thrustFwd * boost;
    if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) accLocal.z += sc.thrustFwd * boost;
    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) accLocal.x -= sc.thrustSide * boost;
    if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) accLocal.x += sc.thrustSide * boost;
    if (glfwGetKey(win, GLFW_KEY_SPACE) == GLFW_PRESS)        accLocal.y += sc.thrustUp * boost;
    if (glfwGetKey(win, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) accLocal.y -= sc.thrustUp * boost;
    //Close window (exit program)
    if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(win, true);

    glm::vec3 accWorld = glm::mat3_cast(tr.rotation) * accLocal;
    ve.linear += accWorld * dt;

    // roll with Q/E
    float roll = 0.0f;
    if (glfwGetKey(win, GLFW_KEY_Q) == GLFW_PRESS) roll += sc.rollSpeed;
    if (glfwGetKey(win, GLFW_KEY_E) == GLFW_PRESS) roll -= sc.rollSpeed;
    ve.angular += glm::vec3(0,0,roll) * dt;

    // simple damping
    ve.linear  *= (1.0f - sc.dampingLin * dt);
    ve.angular *= (1.0f - sc.damingAng * dt);
}

