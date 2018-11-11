#include "positionattr.hpp"
#include "skin/widgetdata.hpp"
#include <QPoint>
#include <QSize>
#include <QDebug>

// Coordinate

Coordinate::Coordinate(Type type, int value)
    : m_type(type)
    , m_value(value)
{
}

Coordinate::Coordinate(const QString &str)
{
    parseStr(str);
}
void Coordinate::parseStr(const QString& str)
{
    if (str == "center") {
        m_type = Center;
    } else if (str.endsWith("%")) {
        m_type = Percent;
        m_value = str.leftRef(str.size() - 1).toInt();
    } else {
        m_type = Pixel;
        m_value = str.toInt();
    }
}

QString Coordinate::toStr() const
{
    if (m_type == Center) {
        return "center";
    } else if (m_type == Percent) {
        return QString("%1%").arg(m_value);
    } else {
        return QString("%1").arg(m_value);
    }
}

int Coordinate::getInt(int my_size, int parent_size) const
{
    if (m_type == Center) {
        return (parent_size - my_size) / 2;
    } else {
        return m_value;
    }
}

void Coordinate::parseInt(int value, int my_size, int parent_size)
{
    if (value * 2 == parent_size - my_size) {
        // exact match, set Center
        m_type = Center;
    } else if (m_type == Center && value == (parent_size - my_size) / 2) {
        // keep Center
    } else {
        m_type = Pixel;
        m_value = value;
    }
}

bool Coordinate::isRelative() const
{
    return m_type == Center;
}


// PositionAttr

PositionAttr::PositionAttr(const Coordinate &x, const Coordinate &y)
    : mX(x), mY(y)
{
}

QPoint PositionAttr::toPoint(const WidgetData& widget) const
{
    QSize s = widget.selfSize();
    QSize p = widget.parentSize();
    return QPoint(mX.getInt(s.width(), p.width()), mY.getInt(s.height(), p.height()));
}

void PositionAttr::setPoint(const WidgetData& widget, const QPoint& pos)
{
    // Try to be smart
    const QSize s = widget.selfSize();
    const QSize p = widget.parentSize();
    mX.parseInt(pos.x(), s.width(), p.width());
    mY.parseInt(pos.y(), s.height(), p.height());
}

QString PositionAttr::toStr() const
{
    return mX.toStr() + "," + mY.toStr();
}
void PositionAttr::fromStr(const QString& str)
{
    QStringList list = str.split(",");
    if (list.length() == 2) {
        mX.parseStr(list[0]);
        mY.parseStr(list[1]);
    } else {
        qWarning() << "bad position:" << str;
    }
}
