#include "xmlnode.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

XmlData::XmlData()
{

}

bool XmlData::nextXmlChild(QXmlStreamReader &xml)
{
    typedef QXmlStreamReader::TokenType TokenType;

    while (true) {
        TokenType t = xml.readNext();
        if (t == TokenType::StartElement || t == TokenType::Comment) {
            return true;
        } else if (t == TokenType::Characters) {
            if (xml.isWhitespace()) {
                continue;
            } else {
                return true;
            }
        } else if (t == TokenType::EndElement) {
            return false;
        }
        break;
    }

    xml.raiseError("Error: unexpected token " + xml.tokenString());
    return false;
}


// RawXmlElement

void RawXmlElement::toXml(QXmlStreamWriter &xml) const
{
    xml.writeStartElement(mTag);
    xml.writeAttributes(mAttrs);
    for (const auto &child: mChilds) {
        child.toXml(xml);
    }
    xml.writeEndElement();
}

void RawXmlElement::fromXml(QXmlStreamReader &xml)
{
    Q_ASSERT(xml.isStartElement());

    mTag = xml.name().toString();
    mAttrs = xml.attributes();

    while (nextXmlChild(xml)) {
        if (xml.isStartElement()) {
            RawXmlElement e;
            e.fromXml(xml);
            mChilds.append(e);
        }
    }
}
