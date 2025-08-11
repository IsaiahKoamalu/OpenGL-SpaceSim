#include "Systems/RenderSystem.h"
#include "Components/Transform.h"
#include "Components/MeshComponent.h"
#include "Components/Material.h"
#include "Components/Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

static glm::mat4 modelFrom(const Transform& t)
{
    return glm::translate(glm::mat4(1), t.position)
        * glm::mat4_cast(t.rotation)
        * glm::scale(glm::mat4(1), t.scale);
}

CameraMatrices computeCamera(const Registry& reg, Entity cameraEntity)
{
    auto& t = reg.get<Transform>(cameraEntity);
    auto& c = reg.get<Camera>(cameraEntity);
    // remember: look forward -Z in local space for now.
    glm::vec3 fwd = glm::mat3_cast(t.rotation) * glm::vec3(0,0,-1);
    glm::vec3 up = glm::mat3_cast(t.rotation) * glm::vec3(0,1,0);;
    glm::vec3 eye = t.position;

    // Note: the aspect is being set in main as of now.
    CameraMatrices cm{};
    cm.view = glm::lookAt(eye, eye+fwd, up);
    cm.proj = glm::mat4(1);
    cm.eyeWorld = eye;
    return cm;
}

void renderSystem(Registry& reg, Shader& shader, const CameraMatrices& cam, const glm::vec3& sunDir)
{
    shader.use();
    shader.setMat4("view", glm::value_ptr(cam.view));
    shader.setMat4("projection", glm::value_ptr(cam.proj));
    shader.setVec3("uSunDir", glm::normalize(sunDir));
    shader.setVec3("uCameraWorld", cam.eyeWorld);

    // ALL renderables
    reg.for_each<Transform, Material, MeshComponent>([&](Entity, Transform& t, Material& m, MeshComponent& mc)
    {
        Shader& sh = m.shader ? *m.shader : shader;
        sh.use();
        glm::mat4 model = modelFrom(t);
        sh.setMat4("model", glm::value_ptr(model));

        // Mat Params
        sh.setVec3("uAlbedoColor", m.albedo);
        sh.setBool("uHasAlbedo", false);
        sh.setBool("uHasNight", false);

        mc.mesh.draw();
    });
}

