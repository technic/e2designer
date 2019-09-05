#pragma once

#include <QXmlStreamWriter>

class XmlStreamWriter : public QXmlStreamWriter
{
public:
    XmlStreamWriter(QIODevice* device)
        : QXmlStreamWriter(device)
    {}
    XmlStreamWriter(QString* string)
        : QXmlStreamWriter(string)
    {}
    void writeQuotedCharacters(const QString& text);
};

