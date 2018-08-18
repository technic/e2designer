#ifndef ATTRCONTAINER_H
#define ATTRCONTAINER_H

#include <QVariant>

/**
 * @brief Map like container to store widget Attributes
 * Values stored in map have variant types
 * You assign specific type to each key during initialization with addAttr()
 */
class AttrContainer
{
    using Container = QHash<int, QVariant>;
public:
    AttrContainer() {}

    template <typename AttrType>
    AttrType getAttr(int key) const;

    template <typename AttrType>
    bool setAttr(int key, const AttrType& value);

    typename Container::const_iterator begin() const noexcept { return mAttrs.cbegin(); }
    typename Container::const_iterator end() const noexcept { return mAttrs.cend(); }

protected:
    template <typename AttrType>
    void addAttr(int key);

private:
    // own
    Container mAttrs;
};

// Implementation
// Current implementation uses QVariant
// If you are on C++17 std::variant can be another possible choice

template <typename AttrType>
void AttrContainer::addAttr(int key)
{
    Q_ASSERT(!mAttrs.contains(key));
    mAttrs[key] = QVariant::fromValue(AttrType());
}

template <typename AttrType>
AttrType AttrContainer::getAttr(int key) const
{
    auto it = mAttrs.find(key);
    if (it != mAttrs.end()) {
        return qvariant_cast<AttrType>(it.value());
    } else {
        return AttrType();
    }
}

template <typename AttrType>
bool AttrContainer::setAttr(int key, const AttrType& value)
{
    auto it = mAttrs.find(key);
    if (it != mAttrs.end() ) {
        if (it.value().userType() == qMetaTypeId<AttrType>()) {
            it.value().setValue(value);
        }
    }
    return false;
}

#endif // ATTRCONTAINER_H
