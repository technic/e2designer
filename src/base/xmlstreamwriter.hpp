#pragma once

#include <QXmlStreamWriter>

class XmlStreamWriter : public QXmlStreamWriter
{
public:
    // This supprots only QIODevice now
    XmlStreamWriter(QIODevice* device)
        : QXmlStreamWriter(device)
    {}
    void writeQuotedCharacters(const QString& text)
    {
        auto parts = text.split('"');
        for (const QString& part : parts) {
            writeCharacters(part);
            if (part != parts.back()) {
                // XML allows to have unquoted characters inside the element
                if (device()) {
                    device()->putChar('"');
                }
            }
        }
    }
};
