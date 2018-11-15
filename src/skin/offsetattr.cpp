#include "offsetattr.hpp"

#include <QVector>
#include <QStringRef>

OffsetAttr::OffsetAttr(int x, int y)
    : m_x(x), m_y(y) {}

OffsetAttr::OffsetAttr(const QString &str)
    : m_x(0), m_y(0)
{
    auto list = str.splitRef(",");
    if (list.size() == 2) {
        m_x = list[0].toInt();
        m_y = list[1].toInt();
    }
}

QString OffsetAttr::toStr() const
{
    if (m_x == 0 && m_y == 0) {
        return QString();
    }
    return QString("%1,%2").arg(m_x).arg(m_y);
}

