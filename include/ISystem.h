#pragma once

class ISystem
{
public:
    virtual ~ISystem() = default;

    virtual void init() {}

    virtual void update(float dt) = 0;

    virtual void shutdown() {}

    void setEnabled(bool e) {m_enabled = e;}
    bool isEnabled() const {return m_enabled;}

protected:
    bool m_enabled = true;
};