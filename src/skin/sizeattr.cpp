#include "sizeattr.hpp"
#include "skin/widgetdata.hpp"
#include "base/meta.hpp"
#include <QDebug>

Dimension::Dimension()
    : m_type(Type::Number), m_value(0)
{
}

Dimension::Dimension(int px)
    : m_type(Type::Number), m_value(px)
{
}

void Dimension::parseStr(const QString& str)
{
    m_value = str.toInt();
}
QString Dimension::toStr() const
{
    switch (m_type) {
    case Type::Number:
        return QString("%1").arg(m_value);
    case Type::Percent:
        return QString("%1%").arg(m_value);
    default:
        return QString("");
    }
}

int Dimension::getInt(int parent_size) const
{
    switch (m_type) {
    case Type::Number:
        return m_value;
    case Type::Percent:
        return m_value * parent_size / 100;
    case Type::Fill:
        return parent_size;
    }
}
void Dimension::setInt(int value)
{
    m_type = Type::Number;
    m_value = value;
}

void Dimension::parseInt(int value, int parent_size)
{
    if (m_type == Type::Number) {
        m_value = value;
    } else if (m_type == Type::Percent) {
        m_value = value * 100 / parent_size;
    }
}

bool Dimension::isRelative() const
{
    return false;
}


// SizeAttr

SizeAttr::SizeAttr(Dimension w, Dimension h)
    : mWidth(w)
    , mHeight(h)
{
}

QSize SizeAttr::getSize(const WidgetData& widget) const
{
    QSize ps = widget.parentSize();
    return QSize(mWidth.getInt(ps.width()), mHeight.getInt(ps.height()));
}

void SizeAttr::setSize(const WidgetData& widget, const QSize size)
{
    QSize s = widget.parentSize();
    mWidth.parseInt(size.width(), s.width());
    mHeight.parseInt(size.height(), s.height());
}

QString SizeAttr::toStr() const
{
    return mWidth.toStr() + "," + mHeight.toStr();
}
void SizeAttr::fromStr(const QString& str)
{
    QStringList list = str.split(",");
    if (list.length() == 2) {
        mWidth.parseStr(list[0]);
        mHeight.parseStr(list[1]);
    } else {
        qDebug() << "bad position:" << str;
    }
}

static ConverterRegistrator cr(&SizeAttr::toStr);

