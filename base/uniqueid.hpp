#ifndef UNIQUEID_H
#define UNIQUEID_H

#include <QtGlobal>
#include <limits>

class UniqueId
{
public:
    UniqueId()
        : mId(getNextId())
    {
    }
    // TODO: release id from possible id pool
    ~UniqueId() {}
    // copy has new id

    UniqueId(const UniqueId& other)
        : mId(getNextId())
    {
        Q_UNUSED(other);
    }
    UniqueId& operator=(const UniqueId& other)
    {
        Q_UNUSED(other);
        mId = getNextId();
        return *this;
    }

    // move has same id

    UniqueId(UniqueId&& other)
        : mId(other.mId)
    {
    }
    UniqueId& operator=(const UniqueId&& other)
    {
        // prevent self-move
        if (this == &other) {
            return *this;
        }
        mId = other.mId;
        return *this;
    }

    uint id() const { return mId; }

private:
    static uint nextId;
    static uint getNextId()
    {
        // TODO: if you need more ids you should somehow reuse
        Q_ASSERT(nextId < std::numeric_limits<uint>::max());
        return nextId++;
    }
    uint mId;
};

#endif // UNIQUEID_H
