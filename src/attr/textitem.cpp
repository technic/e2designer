#include "textitem.hpp"
#include "skin/widgetdata.hpp"

QVariant TextItem::data(int role) const
{
    switch (role) {
    case Roles::GraphicsRole:
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Roles::XmlRole:
        return pWidget->getAttr<TextAttr>(key());
    default:
        return AttrItem::data(role);
    }
}

bool TextItem::setData(const QVariant& value, int role)
{
    switch (role) {
    case Roles::GraphicsRole:
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Roles::XmlRole:
        pWidget->setAttr(key(), value.toString());
        return true;
    default:
        return AttrItem::setData(value, role);
    }
}

static AttrItemRegistrator<TextAttr, TextItem> registrator;
