#include "Systems/RenderSystem.h"
#include "Components/Transform.h"
#include "Components/MeshComponent.h"
#include "Components/Material.h"
#include "Components/Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

#include "Components/DirectionalLight.h"
#include "Components/PointLight.h"

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
    glm::vec3 plPos{0.0f};
    glm::vec3 plColor{1.0f};
    float plIntensity = 0.0f;
    float plRadius = 1.0f;
    bool hasPL = false;

    reg.for_each<PointLight, Transform>([&](Entity, PointLight& pl, Transform& lt)
    {
        if (!hasPL)
        {
            plPos = lt.position;
            plColor = pl.color;
            plIntensity = pl.intensity;
            plRadius = pl.radius;
            hasPL = true;
        }
    });

    // ALL renderables
    reg.for_each<Transform, Material, MeshComponent>([&](Entity, Transform& t, Material& m, MeshComponent& mc)
    {
        Shader& sh = m.shader ? *m.shader : shader;
        sh.use();
        sh.setMat4("view", glm::value_ptr(cam.view));
        sh.setMat4("projection", glm::value_ptr(cam.proj));
        sh.setVec3("uCameraWorld", cam.eyeWorld);
        if (hasPL)
        {
            sh.setVec3("uPL.position", plPos);
            sh.setVec3("uPL.color", plColor);
            sh.setFloat("uPL.intensity", plIntensity);
            sh.setFloat("uPL.radius", plRadius);
        }

        glm::mat4 model = glm::translate(glm::mat4(1), t.position)
                        * glm::mat4_cast(t.rotation)
                        * glm::scale(glm::mat4(1), t.scale);
        sh.setMat4("model", glm::value_ptr(model));

        // Mat Params
        sh.setVec3("uAlbedoColor", m.albedo);
        sh.setBool("uHasAlbedo", false);
        sh.setBool("uHasNight", false);

        if (m.isEmissive)
        {
            sh.setVec3("uEmissiveColor", m.emissiveColor);
            sh.setFloat("uEmissiveIntensity", m.emissiveIntensity);
            sh.setBool("uUseEmissiveTex", m.useEmissiveTex);

            if (m.useEmissiveTex && m.emissiveTex != 0)
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, m.emissiveTex);
                sh.setInt("uEmissiveTex", 0);
            }
        }

        mc.mesh.draw();
    });
}

