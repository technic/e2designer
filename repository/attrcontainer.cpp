#include "attrcontainer.h"

AttrContainer::AttrContainer()
{

}

QVariant AttrContainer::getVariant(const int key) const
{
    ContainerBase *attr = mAttrs[key];
    if (attr) {
        return attr->getVariant();
    }
    return QVariant();
}

bool AttrContainer::setFromVariant(const int key, const QVariant &value)
{
    ContainerBase *attr = mAttrs[key];
    if (attr) {
        return attr->setFromVariant(value);
    }
    return false;
}

AttrContainer::~AttrContainer()
{
    qDeleteAll(mAttrs);
}
