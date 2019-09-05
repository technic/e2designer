#pragma once

#include <QVector>

/**
 * @brief Stores list of (key, value) pairs
 * the order of keys is preserved
 * only unique key names are allowed, except unlimited number of null items
 * provides list like and map like interfaces
 * This is a helper class, mutating methods declared as protected
 */
template<class T>
class NamedList
{
public:
    explicit NamedList() {}
    virtual ~NamedList() {}
    static_assert(std::is_default_constructible<T>(), "T must be default constructible");

protected:
    // list like interface
    bool isValidIndex(int i) const { return 0 <= i && i < m_items.size(); }
    bool setItemName(int i, const QString& name);
    bool setItemValue(int i, const typename T::Value& value);

    bool canInsertItem(const T& item);
    bool insertItem(int i, const T& item);
    bool appendItem(T item) { return insertItem(m_items.count(), item); }
    bool canRemoveItems(int position, int count);
    bool removeItems(int position, int count);
    QVector<T> takeItems(int position, int count);

    bool canMoveItems(int sourcePosition, int count, int destinationPosition);
    bool moveItems(int sourcePosition, int count, int destinationPosition);

public:
    bool contains(const QString& name) const;
    T getValue(const QString& name, const T& defaultValue = T()) const;
    QString getName(const typename T::Value value) const;

    inline int itemsCount() const { return m_items.count(); }
    inline const T& itemAt(int i) const { return m_items[i]; }
    int getIndex(const QString& name) const;

    typename QVector<T>::const_iterator begin() const noexcept { return m_items.cbegin(); }
    typename QVector<T>::const_iterator end() const noexcept { return m_items.cend(); }

protected:
    virtual void emitValueChanged(const QString& name, const T& value) const = 0;

private:
    QVector<T> m_items;
};

// Implementation

template<typename T>
bool NamedList<T>::setItemName(int i, const QString& name)
{
    if (!isValidIndex(i)) {
        return false;
    }
    if (contains(name)) {
        return false;
    }
    emitValueChanged(m_items[i].name(), T());
    m_items[i] = T(name, m_items[i].value());
    emitValueChanged(name, m_items[i]);
    return true;
}

template<typename T>
bool NamedList<T>::setItemValue(int i, const typename T::Value& value)
{
    if (!isValidIndex(i)) {
        return false;
    }
    m_items[i] = T(m_items[i].name(), value);
    emitValueChanged(m_items[i].name(), m_items[i]);
    return true;
}

template<typename T>
bool NamedList<T>::canInsertItem(const T& item)
{
    // Allow to have multiple null items
    if (!item.isNull() && contains(item.name())) {
        return false;
    }
    return true;
}

template<typename T>
bool NamedList<T>::insertItem(int i, const T& item)
{
    if (canInsertItem(item)) {
        m_items.insert(i, item);
        emitValueChanged(item.name(), item);
        return true;
    }
    return false;
}

template<typename T>
bool NamedList<T>::canRemoveItems(int position, int count)
{
    if (position < 0 || position + count > m_items.size()) {
        return false;
    }
    return true;
}

template<typename T>
bool NamedList<T>::removeItems(int position, int count)
{
    if (canRemoveItems(position, count)) {
        for (int i = 0; i < count; ++i) {
            emitValueChanged(m_items[position].name(), T());
            m_items.removeAt(position);
        }
        return true;
    }
    return false;
}

template<class T>
QVector<T> NamedList<T>::takeItems(int position, int count)
{
    if (canRemoveItems(position, count)) {
        QVector<T> result;
        result.reserve(count);
        for (int i = 0; i < count; ++i) {
            emitValueChanged(m_items[position].name(), T());
            result.append(m_items.takeAt(position));
        }
        return result;
    }
    return QVector<T>();
}

template<class T>
bool NamedList<T>::canMoveItems(int sourcePosition, int count, int destinationPosition)
{
    bool overlap =
      destinationPosition >= sourcePosition && destinationPosition <= sourcePosition + count;
    if (sourcePosition >= 0 && sourcePosition + count <= m_items.size() && destinationPosition >= 0
        && destinationPosition <= m_items.size() && !overlap) {
        return true;
    }
    return false;
}

template<class T>
bool NamedList<T>::moveItems(int sourcePosition, int count, int destinationPosition)
{
    if (canMoveItems(sourcePosition, count, destinationPosition)) {
        QVector<T> tmp = m_items.mid(sourcePosition, count);
        for (int i = 0; i < count; ++i) {
            m_items.insert(destinationPosition + i, tmp[i]);
        }
        if (destinationPosition < sourcePosition) {
            m_items.remove(sourcePosition + count, count);
        } else {
            m_items.remove(sourcePosition, count);
        }
        return true;
    }
    return false;
}

template<typename T>
bool NamedList<T>::contains(const QString& name) const
{
    // TODO: optimization with QHash?
    for (const T& item : m_items) {
        if (item.name() == name) {
            return true;
        }
    }
    return false;
}

template<typename T>
T NamedList<T>::getValue(const QString& name, const T& defaultValue) const
{
    // TODO: optimization with QHash?
    for (const T& item : m_items) {
        if (item.name() == name)
            return item;
    }
    return defaultValue;
}

template<class T>
QString NamedList<T>::getName(const typename T::Value value) const
{
    for (const T& item : m_items) {
        if (item.value() == value)
            return item.name();
    }
    return QString();
}

