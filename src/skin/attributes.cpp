#include "attributes.hpp"
#include <QVariant>

QString serialize(int value)
{
    if (value != 0) {
        return QString::number(value);
    } else {
        return QString();
    }
}

void deserialize(const QString& str, int& value)
{
    value = str.toInt();
}

QString serialize(const QString& str)
{
    return str;
}

void deserialize(const QString& str, QString& value)
{
    value = str;
}

QString serialize(bool b)
{
    if (b) {
        return QString("1");
    } else {
        return QString();
    }
}

void deserialize(const QString& str, bool& b)
{
    b = str.toInt();
}

QString serialize(const QVariant& value)
{
    return value.toString();
}

void deserialize(const QString& str, QVariant& value)
{
    value = str;
}
