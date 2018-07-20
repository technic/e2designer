#include "fontitem.hpp"
#include "repository/skinrepository.hpp"
#include <QFontDatabase>
#include <QString>
#include <QStringList>

// FontAttr

QFont FontAttr::getFont() const
{
    QFont f = SkinRepository::fonts()->getValue(mName).font();
    f.setPixelSize(mSize);
    return f;
}

QString FontAttr::toStr() const
{
    if (!mName.isNull() && mSize > 0)
        return QString("%1;%2").arg(mName).arg(mSize);
    else
        return QString();
}

void FontAttr::fromStr(const QString& str)
{
    QStringList l = str.split(';');
    if (l.count() != 2)
        return;
    mName = l[0].trimmed();
    mSize = l[1].toInt();
}

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
