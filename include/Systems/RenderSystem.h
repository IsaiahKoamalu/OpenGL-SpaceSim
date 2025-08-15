#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ISystem.h"
#include "Core/Registry.h"
#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Components/Drawable.h"
#include "Components/PointLight.h"
#include "Components/Skybox.h"
#include "Systems/CameraSystem.h"
#include "Rendering/RenderBackend.h"

class RenderSystem : public ISystem {
public:
    RenderSystem(Registry* reg, RenderBackend* backend, int width, int height)
        : r(reg), gfx(backend)
    {
        setViewport(width, height);
    }

    void setViewport(int width, int height) {
        if (width <= 0)  width  = 1;
        if (height <= 0) height = 1;
        proj = glm::perspective(glm::radians(60.0f), float(width)/float(height), 0.1f, 5000.0f);
    }

    void update(float dt) override {
        Camera cam{}; bool haveCam=false;
        r->view<Camera>([&](Entity, Camera& c){ if(!haveCam){ cam=c; haveCam=true; } });
        if (!haveCam) return;

        const glm::mat4 view = CameraSystem::viewFrom(cam);

        glm::vec3 lightPosWS{0.0f};   // stays in WORLD space
        glm::vec3 lightColor{1.0f};
        float      lightIntensity = 800.0f;
        bool haveLight=false;
        r->view<PointLight, Transform>([&](Entity, PointLight& L, Transform& T){
            if (!haveLight) {
                lightPosWS     = T.position;  // <-- RAW world position
                lightColor     = L.color;
                lightIntensity = L.intensity;
                haveLight = true;
            }
        });

        r->view<Drawable, Transform>([&](Entity, Drawable& d, Transform& t){
            glm::mat4 M(1.0f);
            M = glm::translate(M, t.position);
            M = glm::rotate(M, t.rotationEuler.y, glm::vec3{0,1,0});
            M = glm::rotate(M, t.rotationEuler.x, glm::vec3{1,0,0});
            M = glm::rotate(M, t.rotationEuler.z, glm::vec3{0,0,1});
            M = glm::scale(M, t.scale);

            // Backend uploads all uniforms (including world-space light & cam pos) and draws
            gfx->drawMesh(d.meshId, RenderBackend::DrawParams{
                M, view, proj, d.albedoColor,
                lightPosWS, lightColor, lightIntensity,
                cam.pos
            });
        });

        // Drawing skybox
        int skyTex = -1;
        r->view<Skybox>([&](Entity, Skybox& s)
        {
            if (skyTex < 0)
            {
                skyTex = s.cubeMap;
            }
        });
        if (skyTex > 0)
        {
            gfx->drawSkyBox(view, proj, skyTex);
        }
    }

private:
    Registry* r{};
    RenderBackend* gfx{};
    glm::mat4 proj{1.0f};
};
