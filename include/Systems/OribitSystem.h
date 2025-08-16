#ifndef ORBITSYSTEM_H
#define ORBITSYSTEM_H

#include <glm/gtc/constants.hpp>
#include "ISystem.h"
#include "Components/Orbit.h"
#include "Components/Transform.h"
#include "Core/Registry.h"

class OrbitSystem : public ISystem
{
public:
    explicit OrbitSystem(Registry* reg) : r(reg){}

    void update(float dt) override
    {
        r->view<Orbit, Transform>([&](Entity, Orbit& o, Transform& t)
        {
            if (!o.preComputed)
            {
                o.n = std::sqrt(o.mu / (o.a * o.a * o.a));
                o.preComputed = true;
            }

            // Advance body sim clock
            o.t = (static_cast<double>(dt)) * o.timeScale;

            glm::dvec3 worlPos = keplerPosition(o, o.t);

            t.position = glm::vec3(worlPos);
        });
    }
    static inline double wrapPi(double x)
    {
        const double PI = 3.14159265358979323846;
        x = fmod(x + PI, 2.0*PI);
        if (x < 0)
        {
            x += 2.0 * PI;
        }
        return x - PI;
    }

    glm::dvec3 keplerPosition(const Orbit& k, double tNow)
    {
        const double M = wrapPi(k.M0 + k.n * (tNow - k.t0));

        // Kepler solved for E
        double E = (k.e < 0.8) ? M : glm::pi<double>();
        for (int it = 0; it < 10; ++it)
        {
            const double s = std::sin(E);
            const double c = std::cos(E);
            double f = E - k.e * s - M;
            double fp = 1.0 - k.e * c;
            E -= f / fp;
        }

        const double cosE = std::cos(E);
        const double sinE = std::sin(E);
        const double r = k.a * (1.0 - k.e * cosE);

        const double nu = std::atan2(std::sqrt(1.0 - k.e*k.e) * sinE, cosE - k.e);

        // Perifocal frame position
        glm::dvec3 r_pf(r * std::cos(nu), r * std::sin(nu), 0.0);

        const double cO = std::cos(k.Omega), sO = std::sin(k.Omega);
        const double ci = std::cos(k.i), si = std::sin(k.i);
        const double co = std::cos(k.omega), so = std::sin(k.omega);

        const double R11 = cO * co - sO * so * ci;
        const double R12 = -cO * so - sO * co * ci;
        const double R13 = sO * si;
        const double R21 = sO * co + cO * so * ci;
        const double R22 = -sO * so + cO * co * ci;
        const double R23 = -cO * si;
        const double R31 = so * si;
        const double R32 = co * si;
        const double R33 = ci;

        return{
            R11 * r_pf.x + R12 * r_pf.y + R13 * r_pf.z,
            R21 * r_pf.x + R22 * r_pf.y + R23 * r_pf.z,
            R31 * r_pf.x + R32 * r_pf.y + R33 * r_pf.z
        };
    }
private:
    Registry* r{};
};

#endif
