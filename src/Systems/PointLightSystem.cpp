#include "Systems/PointLightSystem.h"
#include "Components/PointLight.h"
#include "Components/Transform.h"

void updatePointLightPositions(Registry& reg)
{
    reg.for_each<PointLight, Transform>([](Entity, PointLight& pl, Transform& t)
    {
        pl.position = t.position;
    });
}