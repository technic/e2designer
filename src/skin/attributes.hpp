#ifndef ATTRIBUTES_HPP
#define ATTRIBUTES_HPP

#include <QtGlobal>
#include <QColor>
#include <QString>

using IntegerAttr = int;
using TextAttr = QString;

QString serialize(int value);
void deserialize(const QString& str, int& value);

QString serialize(const QString &str);
void deserialize(const QString &str, QString &value);

QString serialize(bool b);
void deserialize(const QString &str, bool &b);

QString serialize(const QVariant &value);
void deserialize(const QString &str, QVariant &value);

#endif // ATTRIBUTES_HPP
