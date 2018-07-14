#ifndef SKINMAPS_H
#define SKINMAPS_H

#include <QHash>

// TODO test it

template<class Key, class Value>
class BiMap
{
public:
    BiMap() { }
    void insert(const Key &key, const Value &val)
    {
        m_data[key] = val;
        m_data_value[val] = key;
    }
    bool contains(const Key &key) const {
        return m_data.contains(key);
    }
    int remove(const Key &key) {
        Value value = m_data.value(key);
        m_data_value.remove(value);
        return m_data.remove(key);
    }
    Value value(const Key &key) const {
        return m_data.value(key);
    }
    Key key(const Value &value) const {
        return m_data_value.value(value);
    }

private:
    QHash<Key,Value> m_data;
    QHash<Value,Key> m_data_value;

};

#endif // SKINMAPS_H
