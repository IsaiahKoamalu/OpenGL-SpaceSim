#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include "Core/Registry.h"
#include "Systems/RenderSystem.h"
#include "Systems/CameraFollowSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/IntegrateSystem.h"
#include "Systems/PointLightSystem.h"
#include "Components/Transform.h"
#include "Components/MeshComponent.h"
#include "Components/Material.h"
#include "Components/Camera.h"
#include "Components/Velocity.h"
#include "Components/ShipControl.h"
#include "Components/PointLight.h"
#include "Rendering/Shader.h"
#include "Rendering/Mesh.h"

static void framebuffer_size(GLFWwindow*, int w, int h){glViewport(0,0,w,h);}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(2560, 1900, "Space Sim", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glClearColor(0.05f, 0.06f, 0.08f, 1.0f);

    // Shader
    Shader planetShader("shaders/planet.vert", "shaders/planet.frag");
    Shader sunShader("shaders/sun.vert", "shaders/sun.frag");

    // Registry (obv)
    Registry registry;

    // Camera entity
    Entity camE = registry.create();
    registry.emplace<Transform>(camE, Transform{glm::vec3(0,1.8f, 5) });
    registry.emplace<Camera>(camE, Camera{60.f, 0.1f, 1000.f, true});

    Entity ship = registry.create();
    registry.emplace<Transform>(ship, Transform{glm::vec3(0, 1.5f, 0)});
    registry.emplace<Velocity>(ship, Velocity{});
    registry.emplace<ShipControl>(ship, ShipControl{});

    registry.get<Transform>(camE) = Transform{ glm::vec3(0, 1.6f, 5) };

    // Planet entity
    Entity planet = registry.create();
    MeshComponent planetMesh{Mesh::fromSphere(2.0, 64, 128)};
    registry.emplace<MeshComponent>(planet, planetMesh);
    registry.emplace<Transform>(planet, Transform{glm::vec3(15, 0, 0)});
    registry.emplace<Material>(planet, Material{&planetShader});

    // Sun entity
    Entity sun = registry.create();
    MeshComponent sunMesh{Mesh::fromSphere(10.0f, 64, 128)};
    registry.emplace<Transform>(sun, Transform{glm::vec3(0,0,0)});
    registry.emplace<MeshComponent>(sun, sunMesh);
    registry.emplace<Material>(sun, Material{&sunShader, glm::vec3(0.3f, 0.6f, 0.9f)});
    registry.emplace<PointLight>(sun, PointLight{
        .position = registry.get<Transform>(sun).position,
        .color = {1.0f, 0.98f, 0.9f},
        .intensity = 80.0f,
        .radius = 120.0f
    });

    double last = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        double now = glfwGetTime();
        float dt = float(now - last);
        last = now;

        inputSystem(window, registry, ship, dt);
        integrateSystem(registry, dt);
        cameraFollowSystem(registry, camE, ship);
        updatePointLightPositions(registry);

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto cam = computeCamera(registry, camE);
        cam.proj = glm::perspective(glm::radians(registry.get<Camera>(camE).fov),
                                    (h>0? float(w)/h : 4.f/3.f),
                                    registry.get<Camera>(camE).nearPlane,
                                    registry.get<Camera>(camE).farPlane);

        glm::vec3 sunDir = glm::normalize(glm::vec3(1.0f, -0.4f, -2.f));
        renderSystem(registry, sunShader, cam, sunDir);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}