#ifndef POINTLIGHTSYSTEM_H
#define POINTLIGHTSYSTEM_H

#include "Core/Registry.h"

// Updates PointLight components to match their entity's Transform.
void updatePointLightPositions(Registry& reg);

#endif