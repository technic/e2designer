#ifndef XMLSTREAMWRITER_HPP
#define XMLSTREAMWRITER_HPP

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

#endif // XMLSTREAMWRITER_HPP
