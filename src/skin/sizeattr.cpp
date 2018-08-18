#include "sizeattr.hpp"
#include "skin/widgetdata.hpp"
#include <QDebug>

Dimension::Dimension()
{
    // default
    m_type = Number;
    m_value = 0;
}

void Dimension::parseStr(const QString& str)
{
    m_value = str.toInt();
}
QString Dimension::toStr() const
{
    switch (m_type) {
    case Number:
        return QString("%1").arg(m_value);
    case Percent:
        return QString("%1%").arg(m_value);
    default:
        return QString("");
    }
}

int Dimension::getInt(int parent_size) const
{
    return m_value;
}
void Dimension::setInt(int value)
{
    m_type = Number;
    m_value = value;
}

void Dimension::parseInt(int value, int parent_size)
{
    if (m_type == Number) {
        m_value = value;
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

QSize SizeAttr::getSize(const WidgetData& widget)
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

