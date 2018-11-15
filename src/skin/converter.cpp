#include "converter.hpp"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

// Converter

void Converter::fromXml(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "convert");

    m_text.clear();
    m_type = xml.attributes().value("type").toString();
    xml.readNext();
    if (xml.isCharacters()) {
        m_text = xml.text().toString();
    }
    if (!xml.isEndElement()) {
        xml.skipCurrentElement();
    }
}

void Converter::toXml(QXmlStreamWriter& xml) const
{
    xml.writeStartElement("convert");
    xml.writeAttribute("type", m_type);
    if (!m_text.isEmpty())
        xml.writeCharacters(m_text);
    xml.writeEndElement();
}
