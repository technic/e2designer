#include "borderset.hpp"
#include "skin/enums.hpp"

// Border

Border::Border()
    : m_bp(bpInvalid)
    , m_fname()
{}
void Border::fromXml(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "pixmap");

    m_fname = xml.attributes().value("filename").toString();
    QStringRef bp = xml.attributes().value("pos");
    QMetaEnum meta = QMetaEnum::fromType<Property::BorderPosition>();
    bool ok;
    m_bp = meta.keyToValue(bp.toLatin1().data(), &ok);

    if (!ok) {
        m_bp = bpInvalid;
    }
    xml.skipCurrentElement();
}

void Border::toXml(QXmlStreamWriter& xml) const
{
    if (m_bp == bpInvalid) {
        return;
    }
    xml.writeStartElement("pixmap");
    const char* bpstr = QMetaEnum::fromType<Property::BorderPosition>().valueToKey(m_bp);
    xml.writeAttribute("pos", QString(bpstr));
    xml.writeAttribute("filename", m_fname);
    xml.writeEndElement();
}

void Border::reset()
{
    m_bp = bpInvalid;
    m_fname.clear();
}

// BorderSet

BorderSet::BorderSet()
    : m_bs(bsInvalid)
{
    int count = QMetaEnum::fromType<Property::BorderPosition>().keyCount();
    m_borders.reserve(count);
    for (int i = 0; i < count; ++i) {
        m_borders.append(Border());
    }
}

void BorderSet::fromXml(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "borderset");

    for (Border& b : m_borders) {
        b.reset();
    }

    QStringRef bs = xml.attributes().value("name");
    bool ok;
    QMetaEnum meta = QMetaEnum::fromType<Property::BorderSet>();
    m_bs = meta.keyToValue(bs.toLatin1().data(), &ok);
    if (!ok) {
        xml.skipCurrentElement();
        m_bs = bsInvalid;
        return;
    }

    while (nextXmlChild(xml)) {
        if (xml.isStartElement()) {
            if (xml.name() != "pixmap") {
                xml.skipCurrentElement();
                continue;
            }
            Border b;
            b.fromXml(xml);
            if (b.isValid()) {
                m_borders[b.bp()] = b;
            }
        }
    }
}

void BorderSet::toXml(QXmlStreamWriter& xml) const
{
    if (m_bs == bsInvalid) {
        return;
    }

    bool empty = true;
    for (const Border& b : m_borders) {
        if (b.isValid()) {
            empty = false;
            break;
        }
    }
    if (empty) {
        return;
    }

    xml.writeStartElement("borderset");
    const char* name = QMetaEnum::fromType<Property::BorderSet>().valueToKey(m_bs);
    xml.writeAttribute("name", QString(name));
    for (const Border& b : m_borders) {
        b.toXml(xml);
    }
    xml.writeEndElement();
}

const Border& BorderSet::getBorder(int bp)
{
    return m_borders[bp];
}
