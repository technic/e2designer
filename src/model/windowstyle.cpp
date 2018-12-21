#include "windowstyle.hpp"
#include "colorsmodel.hpp"
#include "base/meta.hpp"
#include <QMetaEnum>
#include <QColor>

// WindowStyleTitle

void WindowStyleTitle::fromXml(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "title");

    position = PositionAttr(xml.attributes().value("offset").toString());
    font = FontAttr(xml.attributes().value("font").toString());
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

    role = strToEnum<ColorRole>(xml.attributes().value("name").toString());
    color = ColorAttr(xml.attributes().value("color").toString(), true);
    xml.skipCurrentElement();
}

void WindowStyleColor::toXml(QXmlStreamWriter& xml) const
{
    xml.writeStartElement("color");
    xml.writeAttribute("name", enumToStr(role));
    xml.writeAttribute("color", color.toXml());
    xml.writeEndElement();
}

// WindowStyle

WindowStyle::WindowStyle()
    : m_colors(roleCount())
{
}

void WindowStyle::fromXml(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "windowstyle");

    m_type = xml.attributes().value("type").toString();
    m_id = xml.attributes().value("id").toInt();
    m_colors = QVector<WindowStyleColor>(roleCount());

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

int WindowStyle::roleCount() {
    return QMetaEnum::fromType<WindowStyleColor::ColorRole>().keyCount();
}

ColorAttr WindowStyle::getColor(WindowStyleColor::ColorRole role)
{
    for (auto& item : m_colors) {
        if (item.role == role) {
            return item.color;
        }
    }
    return ColorAttr();
}

void WindowStyle::setColor(WindowStyleColor::ColorRole role, ColorAttr color)
{
    for (auto& item : m_colors) {
        if (item.role == role) {
            item.color = color;
            return;
        }
    }
    m_colors.append(WindowStyleColor{role, color});
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

// ColorRolesModel

ColorRolesModel::ColorRolesModel(ColorsModel &colors, QObject *parent)
    : QObject(parent)
    , _style(nullptr)
    , _colors(colors)
{
}

void ColorRolesModel::setStlye(WindowStyle *style)
{
    // We are connected and have to disconnect
    if (_style && !style) {
        disconnect(&_colors, &ColorsModel::valueChanged, this, &ColorRolesModel::onColorValueChanged);
    }
    // We are not connected and have to connect
    if (!_style && style) {
        connect(&_colors, &ColorsModel::valueChanged, this, &ColorRolesModel::onColorValueChanged);
    }
    _style = style;
    reload();
}

QColor ColorRolesModel::getQColor(ColorRole role) const
{
    if (_style) {
        // FIXME: optimize it!
        for (const auto &c : _style->m_colors) {
            if (c.role == role) {
                return c.color.getQColor();
            }
        }
    }
    return QColor();
}

void ColorRolesModel::onColorValueChanged(const QString &name, QRgb value)
{
    Q_ASSERT(_style);
    for (auto& item : _style->m_colors) {
        if (item.color.name() == name) {
            item.color.updateValue(value);
            emit colorChanged(item.role, value);
        }
    }
}

void ColorRolesModel::reload()
{
    // Reload cache
    if (!_style) {
        return;
    }
    for (auto& item : _style->m_colors) {
        item.color.reload(_colors);
        emit colorChanged(item.role, item.color.value().rgba());
    }
}
