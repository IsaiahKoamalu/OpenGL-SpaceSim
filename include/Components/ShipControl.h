#ifndef SHIPCONTROL_H
#define SHIPCONTROL_H

#include <glm/ext/scalar_constants.hpp>

struct ShipControl
{
    float thrustFwd = 3.f;
    float thrustSide = 3.f;
    float thrustUp = 3.f;
    float mouseSense = 0.09;
    float rollSpeed = 1.8f;
    float dampingLin = 0.4f;
    float damingAng = 0.4f;
    bool boost = false;

    float rotSmoothTau = 0.10f;

    float yaw = -glm::pi<float>() * 0.5f;
    float pitch = 0.0f;
};

#endif