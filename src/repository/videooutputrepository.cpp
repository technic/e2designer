#include "videooutputrepository.hpp"
#include <QDebug>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

VideoOutputRepository::VideoOutputRepository() {}
void VideoOutputRepository::addFromXml(QXmlStreamReader& xml)
{
    VideoOutput out;
    out.fromXml(xml);
    appendItem(out);
}

void VideoOutputRepository::toXml(QXmlStreamWriter& xml) const
{
    for (const VideoOutput& output : *this) {
        output.toXml(xml);
    }
}

void VideoOutputRepository::emitValueChanged(const QString& name, const VideoOutput& value) const
{
    Q_UNUSED(name);
    emit valueChanged(value.id(), value);
}

VideoOutput::VideoOutput(const QString& id, const VideoOutputData& data)
    : m_xres(data.resolution.width())
    , m_yres(data.resolution.height())
    , m_bpp(data.bpp)
    , m_id(id.toInt())
{}

void VideoOutput::fromXml(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "output");
    m_id = xml.attributes().value("id").toInt();

    while (nextXmlChild(xml)) {
        if (xml.isStartElement()) {
            if (xml.name() != "resolution") {
                qWarning() << "unknown tag" << xml.name();
                xml.skipCurrentElement();
                continue;
            }
            m_xres = xml.attributes().value("xres").toInt();
            m_yres = xml.attributes().value("yres").toInt();
            m_bpp = xml.attributes().value("bpp").toInt();
        }
    }
    xml.skipCurrentElement();
}

void VideoOutput::toXml(QXmlStreamWriter& xml) const
{
    xml.writeStartElement("output");
    xml.writeAttribute("id", QString::number(m_id));

    xml.writeStartElement("resolution");
    xml.writeAttribute("xres", QString::number(m_xres));
    xml.writeAttribute("yres", QString::number(m_yres));
    xml.writeAttribute("bpp", QString::number(m_bpp));
    xml.writeEndElement();

    xml.writeEndElement();
}
