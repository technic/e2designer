#include "includefile.hpp"
#include "repository/skinrepository.hpp"

IncludeFile::IncludeFile()
    : WidgetData(WidgetType::Widget)
{}

bool IncludeFile::fromXml(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == tag);

    m_fileName = xml.attributes().value("filename").toString();
    xml.skipCurrentElement();

    auto file_name = SkinRepository::instance().dir().filePath(m_fileName);

    // FIXME: this code is similar to SkinRepository code
    QFile file(file_name);
    bool ok = file.open(QIODevice::ReadOnly);
    if (!ok) {
        xml.raiseError(QObject::tr("Can not find include file %1").arg(m_fileName));
        return false;
    }

    QXmlStreamReader inner_xml(&file);
    inner_xml.readNextStartElement();
    if (inner_xml.name() == "skin") {
        while (inner_xml.readNextStartElement()) {
            if (inner_xml.name() == "screen") {
                auto widget = WidgetData::createFromXml(inner_xml);
                if (widget) {
                    appendChild(widget);
                }
            } else {
                qWarning() << "Unexpected tag in inner xml file" << xml.name();
                inner_xml.skipCurrentElement();
            }
        }
    } else {
        inner_xml.raiseError(QObject::tr("Unknow tag in skin: ") + inner_xml.name());
    }

    if (inner_xml.hasError()) {
        xml.raiseError(inner_xml.errorString());
        return false;
    }
    // Check for read errors
    if (file.error() != QFileDevice::FileError::NoError) {
        xml.raiseError(file.errorString());
        return false;
    }
    file.close();

    return true;
}

void IncludeFile::toXml(XmlStreamWriter& xml) const
{
    xml.writeStartElement(tag);
    xml.writeAttribute("filename", m_fileName);
    xml.writeEndElement();

    auto file_name = SkinRepository::instance().dir().filePath(m_fileName);

    // FIXME: this code is similar to SkinRepository code
    QFile file(file_name);
    bool ok = file.open(QIODevice::WriteOnly);
    if (!ok) {
        qWarning() << file.errorString(); // TODO: abort on error
        return;
    }

    XmlStreamWriter inner_xml(&file);
    inner_xml.setAutoFormatting(true);
    inner_xml.setAutoFormattingIndent(2);
    inner_xml.writeStartDocument();

    inner_xml.writeStartElement("skin");
    for (int i = 0; i < childCount(); ++i) {
        child(i)->toXml(inner_xml);
    }
    inner_xml.writeEndElement();

    file.close();
}
