#ifndef SYSTEMMANAGER_H
#define SYSTEMMANAGER_H

#include "System.h"
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <cassert>

class SystemManager
{
public:
    template<typename T, typename... Args>
    std::shared_ptr<T> registerSystems(Args &&... args)
    {
        std::type_index typeID = typeid(T);
        assert(systems.find(typeID) == systems.end() && "System ALREADY registered");

        auto system = std::make_shared<T>(std::forward<Args>(args)...);
        systems[typeID] = system;
        return system;
    }

    template<typename T>
    std::shared_ptr<T> getSystem()
    {
        std::type_index typeID = typeid(T);
        assert(systems.find(typeID) != systems.end() && "System NOT registered");
        return std::static_pointer_cast<T>(systems[typeID]);
    }

    void entityDestroyed(Entity entity)
    {
        for (auto& [_, system] : systems)
        {
            system->entities.erase(entity);
        }
    }

private:
    std::unordered_map<std::type_index, std::shared_ptr<System>> systems;
};

#endif
