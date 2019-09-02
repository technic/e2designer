#include "fontattr.hpp"
#include "repository/skinrepository.hpp"

QFont FontAttr::getFont() const
{
    QFont f = SkinRepository::fonts()->getValue(m_name).font();
    if (m_size > 0)
        f.setPixelSize(m_size);
    return f;
}

QString FontAttr::toStr() const
{
    if (!m_name.isNull() && m_size > 0)
        return QString("%1;%2").arg(m_name).arg(m_size);
    else
        return QString();
}

void FontAttr::fromStr(const QString& str)
{
    QStringList l = str.split(';');
    if (l.count() != 2)
        return;
    m_name = l[0].trimmed();
    m_size = l[1].toInt();
}
