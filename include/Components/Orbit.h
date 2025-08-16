#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

// Kepler Elements
struct Orbit
{
    double a;
    double e;
    double i;
    double Omega;
    double omega;
    double M0; // at t0 epoch
    double mu; // G*M of central body
    double t0; // epoch (seconds)

    // Runtime
    double t = 0.0;
    double timeScale = 1.0;
    bool preComputed = false;
    double n = 0.0;
};
