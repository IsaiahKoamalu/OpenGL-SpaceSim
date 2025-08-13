#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include <queue>
#include <bitset>
#include <cstdint>
#include <limits>

using Entity = uint32_t;

class EntityManager
{
public:
    EntityManager() = default;

    Entity createEntity()
    {
        if (!m_free.empty())
        {
            Entity e = m_free.back();
            m_free.pop_back();
            m_alive[e] = 1;
            return e;
        }
        Entity e = (Entity)m_alive.size();
        m_alive.push_back(1);
        std::cout << "Entity created" << std::endl;
        return e;
    }

    void deleteEntity(Entity e)
    {
        if (e < m_alive.size() && m_alive[e])
        {
            m_alive[e] = 0;
            m_free.push_back(e);
        }
        std::cout << "Entity deleted" << std::endl;
    }

    bool isAlive(Entity e) const
    {
        return (e < m_alive.size() && m_alive[e]);
    }

    void reserve(size_t n)
    {
        if (m_alive.size() < n) m_alive.resize(n, 0);
    }

    size_t capacity() const
    {
        return m_alive.size();
    }

private:
    std::vector<uint8_t> m_alive;
    std::vector<Entity> m_free;
};

#endif
