#include "integeritem.hpp"
#include "repository/widgetdata.hpp"

QVariant IntegerItem::data(int role) const
{
    switch (role) {
    case Roles::GraphicsRole:
    case Qt::DisplayRole:
    case Qt::EditRole:
        return pWidget->getAttr<int>(key());
    case Roles::XmlRole:
    {
        int value = pWidget->getAttr<int>(key());
        if (value != 0) {
            return QString("%1").arg(value);
        } else {
            return QString();
        }
    }
    default:
        return AttrItem::data(role);
    }
}

bool IntegerItem::setData(const QVariant &value, int role)
{
    switch (role) {
    case Roles::GraphicsRole:
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Roles::XmlRole:
        pWidget->setAttr(key(), value.toInt());
        return true;
    default:
        return AttrItem::setData(value, role);
    }
}

static AttrItemRegistrator<IntegerAttr, IntegerItem> registrator;
