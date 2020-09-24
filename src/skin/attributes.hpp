#pragma once

#include <QtGlobal>
#include <QColor>
#include <QString>
#include <QVariant>

inline QString serialize(int value)
{
    if (value != 0) {
        return QString::number(value);
    } else {
        return QString();
    }
}
inline void deserialize(const QString& str, int& value)
{
    value = str.toInt();
}

inline QString serialize(const QString& str)
{
    return str;
}
inline void deserialize(const QString& str, QString& value)
{
    value = str;
}

inline QString serialize(bool b)
{
    if (b) {
        return QString("1");
    } else {
        return QString();
    }
}
inline void deserialize(const QString& str, bool& b)
{
    b = str.toInt();
}

inline QString serialize(const QVariant& value)
{
    return value.toString();
}
inline void deserialize(const QString& str, QVariant& value)
{
    value = str;
}
