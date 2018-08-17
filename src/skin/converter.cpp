#include "converter.hpp"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

// Converter

void Converter::fromXml(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "convert");

    mText.clear();
    mType = xml.attributes().value("type").toString();
    xml.readNext();
    if (xml.isCharacters()) {
        mText = xml.text().toString();
    }
    if (!xml.isEndElement()) {
        xml.skipCurrentElement();
    }
}

void Converter::toXml(QXmlStreamWriter& xml) const
{
    xml.writeStartElement("convert");
    xml.writeAttribute("type", mType);
    xml.writeCharacters(mText);
    xml.writeEndElement();
}
