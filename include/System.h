#ifndef SYSTEM_H
#define SYSTEM_H

#include <cmath>
#include <set>
#include <cstdint>

struct UpdateContext;

/**
 *@brief Base system class that all other systems derive from.
 */

using Entity = std::uint32_t;

class System
{
public:
    std::set<Entity> entities;

    virtual void update(const UpdateContext& ctxt) = 0;
    virtual ~System() = default;
};

#endif