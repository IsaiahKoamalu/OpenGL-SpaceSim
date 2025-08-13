#pragma once
#include <glm/glm.hpp>
#include "glm/gtc/constants.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "ISystem.h"
#include "Core/Registry.h"
#include "Components/Camera.h"

struct InputState
{
    bool W=false, A=false, S=false, D=false, Space=false, Ctrl = false;
    float mouseDx=0.f, mouseDy=0.f; // Pix since last frame
};

class CameraSystem : public ISystem
{
public:
    explicit CameraSystem(Registry* reg) : r(reg){}

    void setInput(const InputState& in_) {in = in_;}

    void update(float dt) override
    {
        r->view<Camera>([&](Entity, Camera& cam)
        {
            cam.yaw += in.mouseDx * cam.sensitivity;
            cam.pitch += in.mouseDy * cam.sensitivity;
            cam.pitch = glm::clamp(cam.pitch, -1.5f, 1.5f);

            const glm::vec3 fwd
            {
                cosf(cam.pitch) * cosf(cam.yaw),
                sinf(cam.pitch),
                cosf(cam.pitch) * sinf(cam.yaw)
            };
            const glm::vec3 right = glm::normalize(glm::cross(fwd, {0,1,0}));
            const glm::vec3 up = glm::normalize(glm::cross(right, fwd));

            glm::vec3 v(0);
            if (in.W) v += fwd;
            if (in.S) v-= fwd;
            if (in.D) v += right;
            if (in.A) v -= right;
            if (in.Space) v += up;
            if (in.Ctrl) v -= up;
            if (glm::length(v) > 0.f) v = glm::normalize(v);

            cam.pos += v * cam.speed * dt;
        });
    }
private:
    Registry* r{};
    InputState in{};
};