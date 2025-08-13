#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include "EntityManager.h"
#include "Components/Camera.h"
#include "Core/Registry.h"
#include "Systems/CameraSystem.h"


static void framebuffer_size(GLFWwindow*, int w, int h){glViewport(0,0,w,h);}
static bool keyDown(GLFWwindow* w, int k) {return glfwGetKey(w, k) == GLFW_PRESS;}

int main()
{
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(400, 400, "Space Sim", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glClearColor(0.05f, 0.06f, 0.08f, 1.0f);

    EntityManager EM;
    Registry R;

    R.registerComponent<Camera>();

    Entity camE = EM.createEntity();
    R.add<Camera>(camE, Camera{});

    CameraSystem camSys(&R);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    double lastX=0, lastY=0;
    bool firstMouse=true;

    double last = glfwGetTime();
    double acc = 0;
    while (!glfwWindowShouldClose(window))
    {
        double now = glfwGetTime();
        float dt = float(now - last);
        last = now;
        acc += dt;

        double x,y;
        glfwGetCursorPos(window, &x, &y);
        float dx=0.f, dy=0.f;
        if (firstMouse){lastX=x; lastY=y; firstMouse=false;}
        dx = float(x - lastX);
        dy = float(lastY - y);
        lastX = x;
        lastY = y;

        InputState in{};
        in.W = keyDown(window, GLFW_KEY_W);
        in.S = keyDown(window, GLFW_KEY_S);
        in.D = keyDown(window, GLFW_KEY_D);
        in.A = keyDown(window, GLFW_KEY_A);
        in.Space = keyDown(window, GLFW_KEY_SPACE);
        in.Ctrl = keyDown(window, GLFW_KEY_LEFT_CONTROL);
        in.mouseDx = dx;
        in.mouseDy = dy;
        camSys.setInput(in);

        camSys.update(dt);

        glClearColor(0.02f, 0.02f, 0.04f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (acc > 1.0)
        {
            const auto& c = R.get<Camera>(camE);
            std::printf("Cam pos: %.2f %.2f %.2f | yaw=%.2f pitch=%.2f\n",
                        c.pos.x, c.pos.y, c.pos.z, c.yaw, c.pitch);
            acc = 0;
        }
    }
    glfwTerminate();
    return 0;
}