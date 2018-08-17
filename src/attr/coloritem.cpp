#include "coloritem.hpp"
#include "adapter/attritemfactory.hpp"
#include "repository/skinrepository.hpp"
#include <QString>

// ColorItem

QVariant ColorItem::data(int role) const
{
    switch (role) {
    case Roles::DataRole:
        return QVariant::fromValue(attr());
    case Qt::DecorationRole:
    case Roles::GraphicsRole:
    case Qt::EditRole:
        return attr().getColor();
    case Qt::DisplayRole:
        return attr().toStr(false);
    case Roles::XmlRole:
        return attr().toStr(true);
    default:
        return AttrItem::data(role);
    }
}

bool ColorItem::setData(const QVariant& value, int role)
{
    switch (role) {
    case Roles::DataRole:
        setAttr(value.value<ColorAttr>());
        return true;
    case Roles::GraphicsRole:
    case Qt::EditRole:
        setAttr(ColorAttr(value.value<QColor>()));
        return true;
    case Roles::XmlRole:
        setAttr(ColorAttr(value.toString(), true));
        return true;
    default:
        return AttrItem::setData(value, role);
    }
}

ColorAttr ColorItem::attr() const
{
    return pWidget->getAttr<ColorAttr>(key());
}
void ColorItem::setAttr(const ColorAttr& attr)
{
    pWidget->setAttr(key(), attr);
}
static AttrItemRegistrator<ColorAttr, ColorItem> registrator;
