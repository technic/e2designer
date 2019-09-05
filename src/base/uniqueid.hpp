#pragma once

#include <QtGlobal>
#include <limits>

class UniqueId
{
public:
    UniqueId()
        : m_id(getNextId())
    {}
    // TODO: release id from possible id pool
    ~UniqueId() {}
    // copy has new id

    UniqueId(const UniqueId& other)
        : m_id(getNextId())
    {
        Q_UNUSED(other);
    }
    UniqueId& operator=(const UniqueId& other)
    {
        Q_UNUSED(other);
        m_id = getNextId();
        return *this;
    }

    // move has same id

    UniqueId(UniqueId&& other)
        : m_id(other.m_id)
    {}
    UniqueId& operator=(const UniqueId&& other)
    {
        // prevent self-move
        if (this == &other) {
            return *this;
        }
        m_id = other.m_id;
        return *this;
    }

    uint id() const { return m_id; }

private:
    static uint nextId;
    static uint getNextId()
    {
        // TODO: if you need more ids you should somehow reuse
        Q_ASSERT(nextId < std::numeric_limits<uint>::max());
        return nextId++;
    }
    uint m_id;
};
