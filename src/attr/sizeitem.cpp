#include "sizeitem.hpp"
#include "repository/widgetdata.hpp"
#include <QDebug>


// SizeItem

QVariant SizeItem::data(int role) const
{
    switch (role) {
    case Roles::GraphicsRole:
        return pWidget->selfSize();
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Roles::XmlRole:
        return attr().toStr();
    default:
        return AttrItem::data(role);
    }
}

bool SizeItem::setData(const QVariant& value, int role)
{
    switch (role) {
    case Roles::GraphicsRole: {
        auto size = attr();
        size.setSize(*pWidget, value.toSize());
        setAttr(size);
        return true;
    }
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Roles::XmlRole:
        setAttr(SizeAttr(value.toString()));
        return true;
    default:
        return AttrItem::setData(value, role);
    }
}

SizeAttr SizeItem::attr() const
{
    return pWidget->getAttr<SizeAttr>(key());
}
void SizeItem::setAttr(const SizeAttr& attr)
{
    pWidget->setAttr<SizeAttr>(key(), attr);
}
static AttrItemRegistrator<SizeAttr, SizeItem> registrator;
