#include <QString>
#include "attritem.h"

// AttrItem

QVariant AttrItem::keyData(int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        return Property::propertyEnum().valueToKey(mKey);
    default:
        return QVariant();
    }
}

QVariant AttrItem::data(int role) const
{
    return QVariant();
}

bool AttrItem::setData(const QVariant& value, int role)
{
    return false;
}
