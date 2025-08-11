#include "EntityManager.h"

EntityManager::EntityManager()
{
    for (Entity e = 0; e < MAX_ENTITIES; ++e)
    {
        availableEntities.push(e);
    }
}

Entity EntityManager::createEntity()
{
    Entity id = availableEntities.front();
    availableEntities.pop();

    aliveEntities.set(id);

    return id;
}

void EntityManager::deleteEntity(Entity entity)
{
    aliveEntities.reset(entity);

    availableEntities.push(entity);
}
