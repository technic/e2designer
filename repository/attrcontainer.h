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
public:
    AttrContainer();

    template<typename AttrType>
    AttrType getAttr(int key) const;

    template<typename AttrType>
    bool setAttr(int key, const AttrType &value);

    template<typename AttrType>
    void addAttr(int key);

    // QVariant interface
    QVariant getVariant(const int key) const;
    bool setFromVariant(const int key, const QVariant &value);

    ~AttrContainer();

private:

    class ContainerBase {
    private:
        int mTypeId;

    protected:
        ContainerBase(int typeId) : mTypeId(typeId) { }

    public:
        virtual ~ContainerBase() { }
//        virtual ContainerBase* copy() const = 0;

        template<typename T>
        T getValue() const
        {
            if (mTypeId == qMetaTypeId<T>()) {
                return static_cast<const Container<T>*>(this)->getData();
            } else {
                return T();
            }
        }
        template<typename T>
        bool setValue(const T &value)
        {
            if (mTypeId == qMetaTypeId<T>()) {
                static_cast<Container<T>*>(this)->setData(value);
                return true;
            } else {
                return false;
            }
        }

        virtual bool setFromVariant(const QVariant &value) = 0;
        virtual QVariant getVariant() = 0;

        inline int typeId() const { return mTypeId; }
    };

    template<typename T>
    class Container : public ContainerBase {
    public:
        Container(T data)
            : ContainerBase(qMetaTypeId<T>()), mData(data) { }

        Container()
            : Container(T()) { }

//        ContainerBase* copy() const final;

        T getData() const { return mData; }
        void setData(T data) { mData = data; }

        bool setFromVariant(const QVariant &value) final {
            if (value.canConvert<T>()) {
                mData = value.value<T>();
                return true;
            } else {
                return false;
            }
        }

        QVariant getVariant() final {
            return QVariant::fromValue(mData);
        }

    private:
        T mData;
    };

protected:
    // own
    QHash<int, ContainerBase*> mAttrs;

};

// Implementation

template<typename AttrType>
void AttrContainer::addAttr(int key)
{
    Q_ASSERT(!mAttrs.contains(key));
    mAttrs[key] = new Container<AttrType>();
}

template<typename AttrType>
AttrType AttrContainer::getAttr(int key) const
{
    auto it = mAttrs.find(key);
    if (it != mAttrs.end()) {
        return it.value()->getValue<AttrType>();
    } else {
        return AttrType();
    }
}

template<typename AttrType>
bool AttrContainer::setAttr(int key, const AttrType &value)
{
    auto it = mAttrs.find(key);
    if (it != mAttrs.end()) {
        return it.value()->setValue(value);
    } else {
        return false;
    }
}

#endif // ATTRCONTAINER_H
