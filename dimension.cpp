#include "dimension.hpp"

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
