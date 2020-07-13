#include "positionattr.hpp"
#include "skin/widgetdata.hpp"
#include "base/meta.hpp"
#include <QPoint>
#include <QSize>
#include <QDebug>

// Coordinate

Coordinate::Coordinate(Type type, int value)
    : m_type(type)
    , m_value(value)
{}

Coordinate::Coordinate(const QString& str)
    : Coordinate()
{
    parseStr(str);
}
void Coordinate::parseStr(const QString& str)
{
    if (str == "center") {
        m_type = Type::Center;
    } else if (str.endsWith("%")) {
        m_type = Type::Percent;
        m_value = str.leftRef(str.size() - 1).toInt();
    } else {
        m_type = Type::Pixel;
        m_value = str.toInt();
    }
}

QString Coordinate::toStr() const
{
    if (m_type == Type::Center) {
        return "center";
    } else if (m_type == Type::Percent) {
        return QString("%1%").arg(m_value);
    } else {
        return QString("%1").arg(m_value);
    }
}

int Coordinate::getInt(int my_size, int parent_size) const
{
    if (m_type == Type::Center) {
        return (parent_size - my_size) / 2;
    } else {
        return m_value;
    }
}

void Coordinate::parseInt(int value, int my_size, int parent_size)
{
    if (value * 2 == parent_size - my_size) {
        // exact match, set Center
        m_type = Type::Center;
    } else if (m_type == Type::Center && value == (parent_size - my_size) / 2) {
        // one pixel precision, keep Center
    } else {
        m_type = Type::Pixel;
        m_value = value;
    }
}

bool Coordinate::isRelative() const
{
    return m_type == Type::Center;
}

// PositionAttr

PositionAttr::PositionAttr(const Coordinate& x, const Coordinate& y)
    : m_x(x)
    , m_y(y)
{}

QPoint PositionAttr::toPoint(const WidgetData& widget) const
{
    QSize s = widget.selfSize();
    QSize p = widget.parentSize();
    return QPoint(m_x.getInt(s.width(), p.width()), m_y.getInt(s.height(), p.height()));
}

void PositionAttr::setPoint(const WidgetData& widget, const QPoint& pos)
{
    // Try to be smart
    const QSize s = widget.selfSize();
    const QSize p = widget.parentSize();
    m_x.parseInt(pos.x(), s.width(), p.width());
    m_y.parseInt(pos.y(), s.height(), p.height());
}

QString PositionAttr::toStr() const
{
    return m_x.toStr() + "," + m_y.toStr();
}
void PositionAttr::fromStr(const QString& str)
{
    QStringList list = str.split(",");
    if (list.length() == 2) {
        m_x.parseStr(list[0]);
        m_y.parseStr(list[1]);
    } else {
        qWarning() << "bad position:" << str;
    }
}

static ConverterRegistration position_cr(&PositionAttr::toStr);
