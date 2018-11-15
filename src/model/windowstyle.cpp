#include "windowstyle.hpp"
#include <QMetaEnum>

// WindowStyleTitle

void WindowStyleTitle::fromXml(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "title");

    position.fromStr(xml.attributes().value("offset").toString());
    font.fromStr(xml.attributes().value("font").toString());
    xml.skipCurrentElement();
}

void WindowStyleTitle::toXml(QXmlStreamWriter& xml) const
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

void WindowStyleColor::fromXml(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "color");

    name = xml.attributes().value("name").toString();
    color.fromStr(xml.attributes().value("color").toString(), true);
    xml.skipCurrentElement();
}

void WindowStyleColor::toXml(QXmlStreamWriter& xml) const
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
void WindowStyle::fromXml(QXmlStreamReader& xml)
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
        }
    }
}

void WindowStyle::toXml(QXmlStreamWriter& xml) const
{
    xml.writeStartElement("windowstyle");
    xml.writeAttribute("type", m_type);
    xml.writeAttribute("id", QString::number(m_id));
    m_title.toXml(xml);
    for (const WindowStyleColor& c : m_colors) {
        c.toXml(xml);
    }
    m_borderSet.toXml(xml);
    xml.writeEndElement();
}

ColorAttr WindowStyle::getColorAttr(WindowStyle::ColorRole role) const
{
    // FIXME: optimize it!
    auto metaEnum = QMetaEnum::fromType<ColorRole>();
    for (const auto &c : m_colors) {
        bool ok;
        int k = metaEnum.keyToValue(c.name.toLatin1().data(), &ok);
        if (ok && k == role) {
            return c.color;
        }
    }
    return ColorAttr();
}

// WindowStylesList

void WindowStylesList::append(WindowStyle style)
{
    appendItem(style);
}

void WindowStylesList::toXml(QXmlStreamWriter& xml) const
{
    for (const WindowStyle& s : *this) {
        s.toXml(xml);
    }
}

void WindowStylesList::emitValueChanged(const QString &name, const WindowStyle &value) const
{
    emit styleChanged(name, value);
}
