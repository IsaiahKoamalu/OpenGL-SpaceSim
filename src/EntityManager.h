#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include <queue>
#include <bitset>
#include <cstdint>
#include <limits>

using Entity = std::uint32_t;
constexpr Entity InvalidEntity = std::numeric_limits<Entity>::max();

constexpr Entity MAX_ENTITIES = 5000;

class EntityManager
{
public:
    EntityManager();

    Entity createEntity();

    void deleteEntity(Entity entity);

private:
    std::queue<Entity> availableEntities;
    std::bitset<MAX_ENTITIES> aliveEntities;
};

#endif
