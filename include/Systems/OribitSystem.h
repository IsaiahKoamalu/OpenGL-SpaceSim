#ifndef ORBITSYSTEM_H
#define ORBITSYSTEM_H

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
            t.position.x += 0.01;
        });
    }
private:
    Registry* r{};
};

#endif
