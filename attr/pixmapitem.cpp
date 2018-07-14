#include "pixmapitem.h"

bool PixmapItem::setData(const QVariant &value, int role)
{
    // TODO: implement!
    switch (role) {
    case Roles::GraphicsRole:
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Roles::XmlRole:
        return false;
    default:
        return AttrItem::setData(value, role);
    }
}

QVariant PixmapItem::data(int role) const
{
    // TODO: implement!
    switch (role) {
    case Roles::GraphicsRole:
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Roles::XmlRole:
        return "";
    default:
        return AttrItem::data(role);
    }
}
