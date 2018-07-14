#include "windowstyle.h"


// WindowStyleTitle

void WindowStyleTitle::fromXml(QXmlStreamReader &xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "title");

    position.fromStr(xml.attributes().value("offset").toString());
    font.fromStr(xml.attributes().value("font").toString());
    xml.skipCurrentElement();
}

void WindowStyleTitle::toXml(QXmlStreamWriter &xml) const
{
    if (font.name().isEmpty()) {
        return;
    }
    xml.writeStartElement("title");
    xml.writeAttribute("offset", position.toStr());
    xml.writeAttribute("font", font.toStr());
    xml.writeEndElement();
}


// WindowStyleColor

void WindowStyleColor::fromXml(QXmlStreamReader &xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "color");

    name = xml.attributes().value("name").toString();
    color.fromStr(xml.attributes().value("color").toString(), true);
    xml.skipCurrentElement();
}

void WindowStyleColor::toXml(QXmlStreamWriter &xml) const
{
    xml.writeStartElement("color");
    xml.writeAttribute("name", name);
    xml.writeAttribute("color", color.toStr(true));
    xml.writeEndElement();
}


// WindowStyle

WindowStyle::WindowStyle()
{
}

void WindowStyle::fromXml(QXmlStreamReader &xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "windowstyle");

    m_type = xml.attributes().value("type").toString();
    m_id = xml.attributes().value("id").toInt();
    m_colors.clear();

    while (nextXmlChild(xml)) {
        if (xml.isStartElement()) {
            if (xml.name() == "title") {
                m_title.fromXml(xml);
            } else if (xml.name() == "color") {
                WindowStyleColor color;
                color.fromXml(xml);
                m_colors.append(color);
            } else if (xml.name() == "borderset") {
                m_borderSet.fromXml(xml);
            } else {
                xml.skipCurrentElement();
                continue;
            }
        } else {
            xml.skipCurrentElement();
        }
    }
}

void WindowStyle::toXml(QXmlStreamWriter &xml) const
{
    xml.writeStartElement("windowstyle");
    xml.writeAttribute("type", m_type);
    xml.writeAttribute("id", QString::number(m_id));
    m_title.toXml(xml);
    for (const WindowStyleColor &c: m_colors) {
        c.toXml(xml);
    }
    m_borderSet.toXml(xml);
    xml.writeEndElement();
}


// WindowStylesList

void WindowStylesList::append(WindowStyle style)
{
//    appendItem(style);
    mItems.append(style);
}

void WindowStylesList::toXml(QXmlStreamWriter &xml) const
{
    for (const WindowStyle &s: mItems) {
        s.toXml(xml);
    }
}

//void WindowStylesList::emitValueChanged(int id, const WindowStyle &value) const
//{
//    // nothing to do here so far
//}
