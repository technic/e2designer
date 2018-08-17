#include "fontattr.hpp"
#include "repository/skinrepository.hpp"

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
