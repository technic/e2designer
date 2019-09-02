#include "xmlnode.hpp"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

XmlData::XmlData() = default;
bool XmlData::nextXmlChild(QXmlStreamReader& xml)
{
    using TokenType = QXmlStreamReader::TokenType;

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

void RawXmlElement::toXml(QXmlStreamWriter& xml) const
{
    xml.writeStartElement(m_tag);
    xml.writeAttributes(m_attrs);
    for (const auto& child : m_childs) {
        child.toXml(xml);
    }
    xml.writeEndElement();
}

void RawXmlElement::fromXml(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement());

    m_tag = xml.name().toString();
    m_attrs = xml.attributes();

    while (nextXmlChild(xml)) {
        if (xml.isStartElement()) {
            RawXmlElement e;
            e.fromXml(xml);
            m_childs.append(e);
        }
    }
}
