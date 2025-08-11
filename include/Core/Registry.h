#ifndef REGISTRY_H
#define REGISTRY_H

#include "Types.h"
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <type_traits>
#include <stdexcept>

/**
 * @brief Per-component map storage where component T -> unordered_map<Entity, T>
 */
class Registry
{
public:
    Entity create()
    {
        Entity e = ++m_next;
        m_alive.push_back(e);
        return e;
    }
    bool alive(Entity e) const { return e != INVALID_ENTITY && e <= m_next; }

    template<class T, class... Args>
    T& emplace(Entity e, Args&&... args)
    {
        return getStore<T>()[e] = T{std::forward<Args>(args)...};
    }

    template<class T>
    bool has(Entity e) const
    {
        auto& s = getStore<TConst<T>>();
        return s.find(e) != s.end();
    }

    template<class T>
    T& get(Entity e)
    {
        auto& s = getStore<T>();
        auto it = s.find(e);
        if (it == s.end()) throw std::runtime_error("component missing");
        return it->second;
    }

    template<class T>
    const T& get(Entity e) const
    {
        auto& s = getStore<TConst<T>>();
        auto it = s.find(e);
        if (it == s.end()) throw std::runtime_error("component missing");
        return it->second;
    }

    // Loop over entities that contain all components Ts
    template<class... Ts, class Func>
    void for_each(Func&& f)
    {
        for (Entity e : m_alive)
        {
            if ((has<Ts>(e) && ...))
            {
                f(e, get<Ts>(e)...);
            }
        }
    }

private:
    // Storage type-erasure helpers
    template<class T> using Store = std::unordered_map<Entity, T>;
    template<class T> using TConst = std::add_const_t<T>;

    template<class T>
    Store<T>& getStore() const
    {
        auto key = std::type_index(typeid(T));
        auto it = m_stores.find(key);
        if (it == m_stores.end())
        {
            auto* ptr = new Store<T>();
            m_stores[key] = ptr;
            return *static_cast<Store<T>*>(ptr);
        }
        return *static_cast<Store<T>*>(it->second);
    }

    mutable std::unordered_map<std::type_index, void*> m_stores;
    std::vector<Entity> m_alive;
    Entity m_next = 0;
};

#endif
