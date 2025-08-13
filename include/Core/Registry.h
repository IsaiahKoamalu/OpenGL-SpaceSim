#ifndef REGISTRY_H
#define REGISTRY_H

#include <bitset>
#include <functional>

#include "Types.h"
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <type_traits>
#include <stdexcept>

/**
 * @brief Per-component map storage where component T -> unordered_map<Entity, T>
 */

using Entity = uint32_t;

class Registry
{
public:

    static constexpr size_t kMaxComponents = 64;
    using Signature = std::bitset<kMaxComponents>;

    Registry() {m_signatures.resize(1024);}

    template<typename C>
    void registerComponent()
    {
        const size_t id = typeId<C>();
        if (id >= m_erasers.size()) m_erasers.resize(id + 1);
        m_erasers[id] = [this](Entity e)
        {
            auto& p = pool<C>();
            if (e < p.alive.size()) p.alive[e] = 0;
        };
    }

    template <typename C, typename... Args>
    C& add(Entity e, Args&&... args)
    {
        ensureEntitySlot(e);
        auto& p = pool<C>();
        ensurePoolSlot(p, e);

        m_signatures[e].set(typeId<C>());
        p.alive[e] = 1;
        p.data[e] = C{std::forward<Args>(args)...};
        return p.data[e];
    }

    template <typename C>
    void remove(Entity e)
    {
        if (!has<C>(e)) return;
        pool<C>().alive[e] = 0;
        m_signatures[e].reset(typeId<C>());
    }

    void destroyEntity(Entity e)
    {
        if (e >= m_signatures.size()) return;
        auto& sig = m_signatures[e];
        for (size_t i = 0; i < kMaxComponents; ++i)
        {
            if (sig.test(i) && i < m_erasers.size() && m_erasers[i])
            {
                m_erasers[i](e);
            }
        }
        sig.reset();
    }

    template <typename C>
    bool has(Entity e) const
    {
        const auto id = typeId<C>();
        if (e >= m_signatures.size()) return false;
        if (!m_signatures[e].test(id)) return false;
        const auto& p = pool<C>();
        return e < p.alive.size() && p.alive[e];
    }

    template <typename C>
    C& get(Entity e)
    {
        return pool<C>().data[e];
    }

    template <typename C>
    const C& get(Entity e) const
    {
        return pool<C>().data[e];
    }

    template <typename... Cs, typename F>
    void view(F&& f)
    {
        const size_t N = m_signatures.size();
        for (Entity e = 0; e < N; ++e)
        {
            if ((has<Cs>(e) && ...))
            {
                f(e, get<Cs>(e)...);
            }
        }
    }

private:
    template <typename C>
    struct Pool
    {
        std::vector<C> data;
        std::vector<uint8_t> alive;
    };

    template <typename C>
    static size_t typeId()
    {
        static size_t id = s_typeCounter++;
        if (id >= kMaxComponents)
        {
            __builtin_trap();
        }
        return id;
    }

    template <typename C>
    Pool<C>& pool() const
    {
        static Pool<C> p;
        return p;
    }

    void ensureEntitySlot(Entity e)
    {
        if (e >= m_signatures.size()) m_signatures.resize(e + 1);
    }

    template <typename C>
    static void ensurePoolSlot(Pool<C>& p, Entity e)
    {
        if (e >= p.data.size())
        {
            const size_t newSize = e + 1;
            p.data.resize(newSize);
            p.alive.resize(newSize, 0);
        }
    }
private:
    static inline size_t s_typeCounter = 0;

    std::vector<Signature> m_signatures;
    mutable std::vector<std::function<void(Entity)>> m_erasers;
};

#endif
