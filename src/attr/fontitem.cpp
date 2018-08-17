#include "fontitem.hpp"
#include "repository/skinrepository.hpp"
#include <QFontDatabase>
#include <QString>
#include <QStringList>

// FontItem

QVariant FontItem::data(int role) const
{
    switch (role) {
    case Roles::GraphicsRole:
        return attr().getFont();
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Roles::XmlRole:
        return attr().toStr();
    default:
        return AttrItem::data(role);
    }
}

bool FontItem::setData(const QVariant& value, int role)
{
    switch (role) {
    case Roles::GraphicsRole:
        return false;
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Roles::XmlRole:
        setAttr(FontAttr(value.toString()));
        return true;
    default:
        return AttrItem::setData(value, role);
    }
}

FontAttr FontItem::attr() const
{
    return pWidget->getAttr<FontAttr>(key());
}
void FontItem::setAttr(const FontAttr& attr)
{
    pWidget->setAttr<FontAttr>(key(), attr);
}
static AttrItemRegistrator<FontAttr, FontItem> registrator;
